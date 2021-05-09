/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

// ★秀丸クラス
internal sealed partial class hmNETDynamicLib
{
    internal sealed partial class Hidemaru
    {
        public sealed class Macro
        {
            static Macro()
            {
                SetUnManagedDll();
            }

            [DllImport("user32.dll", SetLastError = true)]
            static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, IntPtr szTitle);

            public static bool IsExecuting
            {
                get
                {
                    const int WM_USER = 0x400;
                    const int WM_ISMACROEXECUTING = WM_USER + 167;

                    // 875.02から存在するが、安全を見て875正式版以降とする
                    if (version >= 875.99)
                    {
                        IntPtr hWndHidemaru = WindowHandle;
                        if (hWndHidemaru != IntPtr.Zero)
                        {
                            bool cwch = SendMessage(hWndHidemaru, WM_ISMACROEXECUTING, IntPtr.Zero, IntPtr.Zero);
                            return cwch;
                        }
                    }
                    // 古い状態でも取れる。866以上なら余裕
                    else
                    {
                        IntPtr hWndHidemaru = WindowHandle;
                        if (hWndHidemaru != IntPtr.Zero)
                        {
                            IntPtr hHm32Client = FindWindowEx(hWndHidemaru, IntPtr.Zero, "HM32CLIENT", IntPtr.Zero);
                            bool cwch = SendMessage(hHm32Client, WM_ISMACROEXECUTING, IntPtr.Zero, IntPtr.Zero);
                            return cwch;
                        }

                    }

                    return false;
                }
            }

            public struct ExecResult
            {
                public int Result;
                public string Message;
                public Exception Error;
            }

            public static ExecResult ExecFile(String filename)
            {
                ExecResult result = new ExecResult();
                if (IsExecuting)
                {
                    result.Result = -1;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruMacroIsExecutingException");
                    return result;
                }

                if (!System.IO.File.Exists(filename))
                {
                    result.Result = -1;
                    result.Message = "";
                    result.Error = new System.IO.FileNotFoundException(filename);
                    return result;
                }

                if (version < 875.02)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed875);
                    result.Result = 0;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruNeedVersionException");
                    return result;
                }

                IntPtr hWndHidemaru = WindowHandle;
                if (hWndHidemaru == IntPtr.Zero)
                {
                    result.Result = 0;
                    result.Message = "";
                    result.Error = new NullReferenceException("HidemaruWindowHandleException");
                    return result;
                }

                const int WM_USER = 0x400;
                const int WM_REMOTE_EXECMACRO_FILE = WM_USER + 271;

                StringBuilder sbFileName = new StringBuilder(filename);
                StringBuilder sbRet = new StringBuilder("\x0f0f", 0x0f0f + 1); // 最初の値は帰り値のバッファー
                bool cwch = SendMessage(hWndHidemaru, WM_REMOTE_EXECMACRO_FILE, sbRet, sbFileName);
                if (cwch)
                {
                    result.Result = 1;
                    result.Message = sbRet.ToString();
                    result.Error = null;
                }
                else
                {
                    result.Result = 0;
                    result.Message = sbRet.ToString();
                    result.Error = new InvalidOperationException("HidemaruMacroEvalException");
                }
                return result;
            }

            public static ExecResult ExecEval(String cmd)
            {
                ExecResult result = new ExecResult();
                if (IsExecuting)
                {
                    result.Result = -1;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruMacroIsExecutingException");
                    return result;
                }

                if (version < 875.02)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed875);
                    result.Result = 0;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruNeedVersionException");
                    return result;
                }

                IntPtr hWndHidemaru = WindowHandle;
                if (hWndHidemaru == IntPtr.Zero)
                {
                    result.Result = 0;
                    result.Message = "";
                    result.Error = new NullReferenceException("HidemaruWindowHandleException");
                    return result;
                }

                const int WM_USER = 0x400;
                const int WM_REMOTE_EXECMACRO_MEMORY = WM_USER + 272;

                StringBuilder sbExpression = new StringBuilder(cmd);
                StringBuilder sbRet = new StringBuilder("\x0f0f", 0x0f0f + 1); // 最初の値は帰り値のバッファー
                bool cwch = SendMessage(hWndHidemaru, WM_REMOTE_EXECMACRO_MEMORY, sbRet, sbExpression);
                if (cwch)
                {
                    result.Result = 1;
                    result.Message = sbRet.ToString();
                    result.Error = null;
                }
                else
                {
                    result.Result = 0;
                    result.Message = sbRet.ToString();
                    result.Error = new InvalidOperationException("HidemaruMacroEvalException");
                }
                return result;
            }

            // マクロ文字列の実行。複数行を一気に実行可能
            public static ExecResult Eval(String cmd)
            {
                ExecResult result = new ExecResult();
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    result.Result = 0;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruNeedVersionException");
                    return result;
                }

                int ret = 0;
                try
                {
                    ret = pEvalMacro(cmd);
                }
                catch (Exception e)
                {
                    OutputDebugStream(e.Message);
                }
                if (ret == 0)
                {
                    result.Result = 0;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruMacroEvalException");
                }
                else
                {
                    result.Result = ret;
                    result.Message = "";
                    result.Error = null;
                }
                return result;
            }

            private static int statement_base_random = 0;
            public struct ExecStateResult
            {
                public int Result;
                public string Message;
                public Exception Error;
                public List<Object> Args;
            }
            public static ExecStateResult AsStatementTryInvokeMember(string funcname, params object[] args)
            {
                if (statement_base_random == 0)
                {
                    statement_base_random = new System.Random().Next(Int16.MaxValue) + 1;

                }

                var arg_list = new List<KeyValuePair<String, Object>>();
                int cur_random = new Random().Next(Int16.MaxValue) + 1;
                foreach (var value in args)
                {
                    bool success = false;
                    cur_random++;
                    object normalized_arg = null;
                    // Boolean型であれば、True:1 Flase:0にマッピングする
                    if (value is bool)
                    {
                        success = true;
                        if ((bool)value == true)
                        {
                            normalized_arg = 1;
                        }
                        else
                        {
                            normalized_arg = 0;
                        }
                    }

                    // 32bit
                    if (IntPtr.Size == 4)
                    {
                        // まずは整数でトライ
                        Int32 itmp = 0;
                        success = Int32.TryParse(value.ToString(), out itmp);

                        if (success == true)
                        {
                            normalized_arg = itmp;
                        }

                        else
                        {
                            // 次に少数でトライ
                            Double dtmp = 0;
                            success = Double.TryParse(value.ToString(), out dtmp);
                            if (success)
                            {
                                normalized_arg = (Int32)(dtmp);
                            }

                            else
                            {
                                normalized_arg = 0;
                            }
                        }
                    }

                    // 64bit
                    else
                    {
                        // まずは整数でトライ
                        Int64 itmp = 0;
                        success = Int64.TryParse(value.ToString(), out itmp);

                        if (success == true)
                        {
                            normalized_arg = itmp;
                        }

                        else
                        {
                            // 次に少数でトライ
                            Double dtmp = 0;
                            success = Double.TryParse(value.ToString(), out dtmp);
                            if (success)
                            {
                                normalized_arg = (Int64)(dtmp);
                            }
                            else
                            {
                                normalized_arg = 0;
                            }
                        }
                    }

                    // 成功しなかった
                    if (!success)
                    {
                        normalized_arg = value.ToString();
                    }

                    if (normalized_arg is Int32 || normalized_arg is Int64)
                    {
                        string key = "#AsStatement_" + statement_base_random.ToString() + '_' + cur_random.ToString();
                        arg_list.Add(new KeyValuePair<string, object>(key, normalized_arg));
                        hmNETDynamicLib.Hidemaru.Macro.Var[key] = normalized_arg;
                    }
                    else if (normalized_arg is string)
                    {
                        string key = "$AsStatement_" + statement_base_random.ToString() + '_' + cur_random.ToString();
                        arg_list.Add(new KeyValuePair<string, object>(key, normalized_arg));
                        hmNETDynamicLib.Hidemaru.Macro.Var[key] = normalized_arg;
                    }
                }

                // keyをリスト化
                var arg_keys = new List<String>();
                foreach (var l in arg_list)
                {
                    arg_keys.Add(l.Key);
                }

                // それを「,」で繋げる
                string args_string = String.Join(", ", arg_keys);
                // それを指定の「文」で実行する形
                string expression = $"{funcname} {args_string};\n";

                // 実行する
                ExecResult ret = hmNETDynamicLib.Hidemaru.Macro.Eval(expression);
                ExecStateResult result = new ExecStateResult();
                result.Result = ret.Result;
                result.Error = ret.Error;
                result.Message = ret.Message;
                result.Args = new List<object>();

                // 成否も含めて結果を入れる。
                // new TResult(ret.Result, ret.Message, ret.Error);

                // 使ったので削除
                foreach (var l in arg_list)
                {
                    if (l.Value is Int32 || l.Value is Int64)
                    {
                        result.Args.Add(hmNETDynamicLib.Hidemaru.Macro.Var[l.Key]);
                        hmNETDynamicLib.Hidemaru.Macro.Var[l.Key] = 0;
                    }
                    else if (l.Value is string)
                    {
                        result.Args.Add(hmNETDynamicLib.Hidemaru.Macro.Var[l.Key]);
                        hmNETDynamicLib.Hidemaru.Macro.Var[l.Key] = "";
                    }
                }

                return result;
            }

            private static int funciton_base_random = 0;

            public struct ExecFuncResult
            {
                public object Result;
                public string Message;
                public Exception Error;
                public List<Object> Args;
            }
            public static ExecFuncResult AsFunctionTryInvokeMember(string funcname, params object[] args)
            {
                if (funciton_base_random == 0)
                {
                    funciton_base_random = new System.Random().Next(Int16.MaxValue) + 1;

                }

                var arg_list = new List<KeyValuePair<String, Object>>();
                int cur_random = new Random().Next(Int16.MaxValue) + 1;
                foreach (var value in args)
                {
                    bool success = false;
                    cur_random++;
                    object normalized_arg = null;
                    // Boolean型であれば、True:1 Flase:0にマッピングする
                    if (value is bool)
                    {
                        success = true;
                        if ((bool)value == true)
                        {
                            normalized_arg = 1;
                        }
                        else
                        {
                            normalized_arg = 0;
                        }
                    }

                    // 32bit
                    if (IntPtr.Size == 4)
                    {
                        // まずは整数でトライ
                        Int32 itmp = 0;
                        success = Int32.TryParse(value.ToString(), out itmp);

                        if (success == true)
                        {
                            normalized_arg = itmp;
                        }

                        else
                        {
                            // 次に少数でトライ
                            Double dtmp = 0;
                            success = Double.TryParse(value.ToString(), out dtmp);
                            if (success)
                            {
                                normalized_arg = (Int32)(dtmp);
                            }

                            else
                            {
                                normalized_arg = 0;
                            }
                        }
                    }

                    // 64bit
                    else
                    {
                        // まずは整数でトライ
                        Int64 itmp = 0;
                        success = Int64.TryParse(value.ToString(), out itmp);

                        if (success == true)
                        {
                            normalized_arg = itmp;
                        }

                        else
                        {
                            // 次に少数でトライ
                            Double dtmp = 0;
                            success = Double.TryParse(value.ToString(), out dtmp);
                            if (success)
                            {
                                normalized_arg = (Int64)(dtmp);
                            }
                            else
                            {
                                normalized_arg = 0;
                            }
                        }
                    }

                    // 成功しなかった
                    if (!success)
                    {
                        normalized_arg = value.ToString();
                    }

                    if (normalized_arg is Int32 || normalized_arg is Int64)
                    {
                        string key = "#AsStatement_" + funciton_base_random.ToString() + '_' + cur_random.ToString();
                        arg_list.Add(new KeyValuePair<string, object>(key, normalized_arg));
                        hmNETDynamicLib.Hidemaru.Macro.Var[key] = normalized_arg;
                    }
                    else if (normalized_arg is string)
                    {
                        string key = "$AsStatement_" + funciton_base_random.ToString() + '_' + cur_random.ToString();
                        arg_list.Add(new KeyValuePair<string, object>(key, normalized_arg));
                        hmNETDynamicLib.Hidemaru.Macro.Var[key] = normalized_arg;
                    }
                }

                // keyをリスト化
                var arg_keys = new List<String>();
                foreach (var l in arg_list)
                {
                    arg_keys.Add(l.Key);
                }

                // それを「,」で繋げる
                string args_string = String.Join(", ", arg_keys);
                // それを指定の「文」で実行する形
                string expression = $"{funcname}({args_string})";

                //----------------------------------------------------------------
                ExecFuncResult result = new ExecFuncResult();
                result.Args = new List<object>();

                // 866より少ないのでこのリターンの正常性は考慮しなくても良い
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    result.Result = null;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruNeedVersionException");
                    return result;
                }

                tmpVar = null;
                int dll = iDllBindHandle;

                if (dll == 0)
                {
                    throw new NullReferenceException(ErrorMsg.NoDllBindHandle866);
                }

                String invocate = ModifyFuncCallByDllType("{0}");
                String cmd = "" +
                    "##_tmp_dll_id_ret = dllfuncw( " + invocate + " \"SetTmpVar\", " + expression + ");\n" +
                    "##_tmp_dll_id_ret = 0;\n";

                Eval(cmd);

                bool is_exception = false;
                if (tmpVar == null)
                {
                    is_exception = true;
                    result.Result = null;
                    result.Message = "";
                    result.Error = new InvalidOperationException("HidemaruMacroEvalException");
                }

                if (!is_exception)
                {
                    Object ret = tmpVar;
                    tmpVar = null; // クリア

                    if (ret.GetType().Name != "String")
                    {
                        if (IntPtr.Size == 4)
                        {
                            result.Result = (Int32)ret + 0; // 確実に複製を
                            result.Message = "";
                            result.Error = null;
                        }
                        else
                        {
                            result.Result = (Int64)ret + 0; // 確実に複製を
                            result.Message = "";
                            result.Error = null;
                        }
                    }
                    else
                    {
                        result.Result = (String)ret + ""; // 確実に複製を
                        result.Message = "";
                        result.Error = null;
                    }

                }

                // 使ったので削除
                foreach (var l in arg_list)
                {
                    if (l.Value is Int32 || l.Value is Int64)
                    {
                        result.Args.Add(hmNETDynamicLib.Hidemaru.Macro.Var[l.Key]);
                        hmNETDynamicLib.Hidemaru.Macro.Var[l.Key] = 0;
                    }
                    else if (l.Value is string)
                    {
                        result.Args.Add(hmNETDynamicLib.Hidemaru.Macro.Var[l.Key]);
                        hmNETDynamicLib.Hidemaru.Macro.Var[l.Key] = "";
                    }
                }

                return result;
            }


            // マクロ文字列の実行。複数行を一気に実行可能
            public static TMacroVar Var = new TMacroVar();
            public sealed class TMacroVar
            {
                public TMacroVar() { }
                public Object this[String var_name]
                {
                    get
                    {
                        if (version < 866)
                        {
                            OutputDebugStream(ErrorMsg.MethodNeed866);
                            return null;
                        }

                        tmpVar = null;
                        int dll = iDllBindHandle;

                        if (dll == 0)
                        {
                            throw new NullReferenceException(ErrorMsg.NoDllBindHandle866);
                        }

                        String invocate = ModifyFuncCallByDllType("{0}");
                        String cmd = "" +
                            "##_tmp_dll_id_ret = dllfuncw( " + invocate + " \"SetTmpVar\", " + var_name + ");\n" +
                            "##_tmp_dll_id_ret = 0;\n";

                        Eval(cmd);

                        if (tmpVar == null)
                        {
                            return null;
                        }
                        Object ret = tmpVar;
                        tmpVar = null; // クリア

                        if (ret.GetType().Name != "String")
                        {
                            if (IntPtr.Size == 4)
                            {
                                return (Int32)ret + 0; // 確実に複製を
                            }
                            else
                            {
                                return (Int64)ret + 0; // 確実に複製を
                            }
                        }
                        else
                        {
                            return (String)ret + ""; // 確実に複製を
                        }
                    }

                    set
                    {
                        // 設定先の変数が数値型
                        if (var_name.StartsWith("#"))
                        {
                            if (version < 866)
                            {
                                OutputDebugStream(ErrorMsg.MethodNeed866);
                                return;
                            }

                            int dll = iDllBindHandle;

                            if (dll == 0)
                            {
                                throw new NullReferenceException(ErrorMsg.NoDllBindHandle866);
                            }

                            Object result = new Object();

                            // Boolean型であれば、True:1 Flase:0にマッピングする
                            if (value.GetType().Name == "Boolean")
                            {
                                if ((Boolean)value == true)
                                {
                                    value = 1;
                                }
                                else
                                {
                                    value = 0;
                                }
                            }

                            // 32bit
                            if (IntPtr.Size == 4)
                            {
                                // まずは整数でトライ
                                Int32 itmp = 0;
                                bool success = Int32.TryParse(value.ToString(), out itmp);

                                if (success == true)
                                {
                                    result = itmp;
                                }

                                else
                                {
                                    // 次に少数でトライ
                                    Double dtmp = 0;
                                    success = Double.TryParse(value.ToString(), out dtmp);
                                    if (success)
                                    {
                                        result = (Int32)(dtmp);
                                    }

                                    else
                                    {
                                        result = 0;
                                    }
                                }
                            }

                            // 64bit
                            else
                            {
                                // まずは整数でトライ
                                Int64 itmp = 0;
                                bool success = Int64.TryParse(value.ToString(), out itmp);

                                if (success == true)
                                {
                                    result = itmp;
                                }

                                else
                                {
                                    // 次に少数でトライ
                                    Double dtmp = 0;
                                    success = Double.TryParse(value.ToString(), out dtmp);
                                    if (success)
                                    {
                                        result = (Int64)(dtmp);
                                    }
                                    else
                                    {
                                        result = 0;
                                    }
                                }
                            }

                            SetTmpVar(result);
                            String invocate = ModifyFuncCallByDllType("{0}");
                            String cmd = "" +
                                var_name + " = dllfuncw( " + invocate + " \"PopNumVar\" );\n";
                            Eval(cmd);
                            SetTmpVar(null);
                        }

                        else // if (var_name.StartsWith("$")
                        {
                            if (version < 866)
                            {
                                OutputDebugStream(ErrorMsg.MethodNeed866);
                                return;
                            }

                            int dll = iDllBindHandle;

                            if (dll == 0)
                            {
                                throw new NullReferenceException(ErrorMsg.NoDllBindHandle866);
                            }

                            String result = value.ToString();
                            SetTmpVar(result);
                            String invocate = ModifyFuncCallByDllType("{0}");
                            String cmd = "" +
                                var_name + " = dllfuncstrw( " + invocate + " \"PopStrVar\" );\n";
                            Eval(cmd);
                            SetTmpVar(null);
                        }
                    }
                }
            }
        }
    }


}

