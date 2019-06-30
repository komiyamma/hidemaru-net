/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Runtime.InteropServices;


// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmLmDynamicLib
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

            public static IResult Eval(String cmd)
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    TResult result = new TResult(0, "", new InvalidOperationException("HidemaruNeedVersionException"));
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
                    TResult result = new TResult(ret, "", new InvalidOperationException("HidemaruMacroEvalException"));
                    return result;
                }
                else
                {
                    TResult result = new TResult(ret, "", null);
                    return result;
                }
            }

            // マクロ文字列の実行。複数行を一気に実行可能
            public static TMacroVar Var;
            public class TMacroVar
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
