/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;


// ★秀丸クラス
public sealed partial class hmV8DynamicLib
{
    public sealed partial class Hidemaru
    {
        public sealed class Macro
        {
            static Macro()
            {
                SetUnManagedDll();

            }

            // マクロでの問い合わせ結果系
            public interface IResult
            {
                int Result { get; }
                String Message { get; }
                Exception Error { get; }
            }

            // 問い合わせ結果系の実態。外から見えないように
            private class TResult : IResult
            {
                public int Result { get; set; }
                public string Message { get; set; }
                public Exception Error { get; set; }

                public TResult(int Result, String Message, Exception Error)
                {
                    this.Result = Result;
                    this.Message = Message;
                    this.Error = Error;
                }
            }

            // マクロ文字列の実行。複数行を一気に実行可能
            internal static int _Eval(String cmd)
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return 0;
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
                return ret;
            }

            // マクロ文字列の実行。複数行を一気に実行可能。
            // 文字列なら、そのまま、ぞれ以外なら、「engine.Script.R」の関数でヒアドキュメント化する。
            // function R(text){ で検索
            public static IResult Eval(Object here_document)
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    TResult result = new TResult(0, "", new InvalidOperationException("HidemaruNeedVersionException"));
                    return result;
                }

                // 文字列で書いているようであれば、普通のEval代わりに使っている
                if (here_document.GetType().Name == "String")
                {
                    int _ret = _Eval((String)here_document);

                    if (_ret == 0)
                    {
                        TResult result = new TResult(_ret, "", new InvalidOperationException("HidemaruMacroEvalException"));
                        return result;
                    }
                    else
                    {
                        TResult result = new TResult(_ret, "", null);
                        return result;
                    }
                } else { 

                    String cmd = engine.Script.R(here_document);

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
                        TResult result = new TResult(ret, "", new InvalidOperationException("HidemaruMacroEvalException"));
                        return result;
                    }
                    else
                    {
                        TResult result = new TResult(ret, "", null);
                        return result;
                    }
                }
            }

            private static List<KeyValuePair<string, object>> SetMacroVarAndMakeMacroKeyArray(object[] args, int base_random)
            {
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

                    if (value is string || value is StringBuilder)
                    {
                        success = true;
                        normalized_arg = value.ToString();
                    }

                    if (!success)
                    {
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
                    }

                    // 成功しなかった
                    if (!success)
                    {
                        normalized_arg = value.ToString();
                    }

                    if (normalized_arg is Int32 || normalized_arg is Int64)
                    {
                        string key = "#AsMacroArs_" + base_random.ToString() + '_' + cur_random.ToString();
                        arg_list.Add(new KeyValuePair<string, object>(key, normalized_arg));
                        hmV8DynamicLib.Hidemaru.Macro.__Var(key, normalized_arg);
                    }
                    else if (normalized_arg is string)
                    {
                        string key = "$AsMacroArs_" + base_random.ToString() + '_' + cur_random.ToString();
                        arg_list.Add(new KeyValuePair<string, object>(key, normalized_arg));
                        hmV8DynamicLib.Hidemaru.Macro.__Var(key, normalized_arg);
                    }
                }

                return arg_list;
            }



            private static int statement_base_random = 0;

            // マクロでの問い合わせ結果系
            public interface IStatementResult
            {
                int Result { get; }
                String Message { get; }
                Exception Error { get; }
                List<Object> Args { get; }
            }


            private class TStatementResult : IStatementResult
            {
                public int Result { get; set; }
                public string Message { get; set; }
                public Exception Error { get; set; }
                public List<Object> Args { get; set; }

                public TStatementResult(int Result, String Message, Exception Error, List<Object> Args)
                {
                    this.Result = Result;
                    this.Message = Message;
                    this.Error = Error;
                    this.Args = new List<Object>(Args); // コピー渡し
                }
            }

            private struct ExecStateResult
            {
                public int Result;
                public string Message;
                public Exception Error;
                public List<Object> Args;
            }

            // AsStatementTryInvokeMemberのプロキシを貼るためのショートカット
            public static Object Statement;

            public static IStatementResult __AsStatementTryInvokeMember(string funcname, params object[] args)
            {
                if (statement_base_random == 0)
                {
                    statement_base_random = new System.Random().Next(Int16.MaxValue) + 1;

                }

                List<KeyValuePair<string, object>> arg_list = SetMacroVarAndMakeMacroKeyArray(args, statement_base_random);

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
                IResult ret = hmV8DynamicLib.Hidemaru.Macro.Eval(expression);
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
                        result.Args.Add(hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key));
                        hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key, 0);
                    }
                    else if (l.Value is string)
                    {
                        result.Args.Add(hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key));
                        hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key, "");
                    }
                }

                return new TStatementResult(result.Result, result.Message, result.Error, result.Args); ;
            }

            private static int funciton_base_random = 0;


            // マクロでの問い合わせ結果系
            public interface IFunctionResult
            {
                object Result { get; }
                String Message { get; }
                Exception Error { get; }
                List<Object> Args { get; }
            }

            private class TFunctionResult : IFunctionResult
            {
                public object Result { get; set; }
                public string Message { get; set; }
                public Exception Error { get; set; }
                public List<Object> Args { get; set; }

                public TFunctionResult(object Result, String Message, Exception Error, List<Object> Args)
                {
                    this.Result = Result;
                    this.Message = Message;
                    this.Error = Error;
                    this.Args = new List<Object>(Args); // コピー渡し
                }
            }


            public struct ExecFuncResult
            {
                public object Result;
                public string Message;
                public Exception Error;
                public List<Object> Args;
            }

            // __AsFunctionTryInvokeMemberのプロキシを貼るためのショートカット
            public static Object Function;

            public static IFunctionResult __AsFunctionTryInvokeMember(string funcname, params object[] args)
            {
                if (funciton_base_random == 0)
                {
                    funciton_base_random = new System.Random().Next(Int16.MaxValue) + 1;
                }

                List<KeyValuePair<string, object>> arg_list = SetMacroVarAndMakeMacroKeyArray(args, statement_base_random);

                // keyをリスト化
                var arg_keys = new List<String>();
                foreach (var l in arg_list)
                {
                    arg_keys.Add(l.Key);
                }

                // それを「,」で繋げる
                string args_string = String.Join(", ", arg_keys);
                // それを指定の「関数」で実行する形
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
                    return new TFunctionResult(result.Result, result.Message, result.Error, result.Args); ;
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

                IResult eval_ret = Eval(cmd);

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
                            result.Message = eval_ret.Message;
                            result.Error = eval_ret.Error;
                        }
                        else
                        {
                            result.Result = (Int64)ret + 0; // 確実に複製を
                            result.Message = eval_ret.Message;
                            result.Error = eval_ret.Error;
                        }
                    }
                    else
                    {
                        result.Result = (String)ret + ""; // 確実に複製を
                        result.Message = eval_ret.Message;
                        result.Error = eval_ret.Error;
                    }

                }

                // 使ったので削除
                foreach (var l in arg_list)
                {
                    if (l.Value is Int32 || l.Value is Int64)
                    {
                        result.Args.Add(hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key));
                        hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key, 0);
                    }
                    else if (l.Value is string)
                    {
                        result.Args.Add(hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key));
                        hmV8DynamicLib.Hidemaru.Macro.__Var(l.Key, "");
                    }
                }

                return new TFunctionResult(result.Result, result.Message, result.Error, result.Args); ;
            }



            // __Varのプロキシを貼るためのショートカット
            public static Object Var;

            // マクロ文字列の実行。複数行を一気に実行可能
            public static Object __Var(String var_name, Object value = null)
            {
                // 読み取りであれば…
                if (value == null)
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

                    _Eval(cmd);

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

                    // 書き込みであれば…
                }
                else
                {
                    // 設定先の変数が数値型
                    if (var_name.StartsWith("#"))
                    {
                        if (version < 866)
                        {
                            OutputDebugStream(ErrorMsg.MethodNeed866);
                            return null;
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
                        _Eval(cmd);
                        SetTmpVar(null);

                        return result;
                    }

                    else // if (var_name.StartsWith("$")
                    {
                        if (version < 866)
                        {
                            OutputDebugStream(ErrorMsg.MethodNeed866);
                            return null;
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
                        _Eval(cmd);
                        SetTmpVar(null);

                        return result;
                    }
                }

            }
        }
    }
}
