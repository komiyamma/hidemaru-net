using System;
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

            public static int Exec(String filename)
            {
                if (IsExecuting)
                {
                    return -1;
                }

                if (!System.IO.File.Exists(filename))
                {
                    throw new System.IO.FileNotFoundException(filename);
                }

                const int WM_USER = 0x400;
                const int WM_REMOTE_EXECMACRO_FILE = WM_USER + 271;

                if (version >= 875.02)
                {
                    IntPtr hWndHidemaru = WindowHandle;
                    if (hWndHidemaru != IntPtr.Zero)
                    {
                        StringBuilder sbFileName = new StringBuilder(filename);
                        StringBuilder sbRet = new StringBuilder("\x0f0f", 0x0f0f + 1); // 最初の値は帰り値のバッファー
                        bool cwch = SendMessage(hWndHidemaru, WM_REMOTE_EXECMACRO_FILE, sbRet, sbFileName);
                        if (cwch)
                        {
                            return 1;
                        }
                        else
                        {
                            throw new InvalidOperationException(filename + ":" + sbRet.ToString());
                        }
                    }
                }
                return 0;
            }

            public static int ExecEval(String cmd)
            {
                if (IsExecuting)
                {
                    return -1;
                }

                const int WM_USER = 0x400;
                const int WM_REMOTE_EXECMACRO_MEMORY = WM_USER + 272;

                if (version >= 875.02)
                {
                    IntPtr hWndHidemaru = WindowHandle;
                    if (hWndHidemaru != IntPtr.Zero)
                    {
                        StringBuilder sbExpression = new StringBuilder(cmd);
                        StringBuilder sbRet = new StringBuilder("\x0f0f", 0x0f0f + 1); // 最初の値は帰り値のバッファー
                        bool cwch = SendMessage(hWndHidemaru, WM_REMOTE_EXECMACRO_MEMORY, sbRet, sbExpression);
                        if (cwch)
                        {
                            return 1;
                        }
                        else
                        {
                            throw new InvalidOperationException("ExecEval:" + sbRet.ToString());
                        }
                    }
                }
                return 0;
            }

            // マクロ文字列の実行。複数行を一気に実行可能
            public static int Eval(String cmd)
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
                                        result = (Int32)Math.Floor(dtmp);
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
                                        result = (Int64)Math.Floor(dtmp);
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

