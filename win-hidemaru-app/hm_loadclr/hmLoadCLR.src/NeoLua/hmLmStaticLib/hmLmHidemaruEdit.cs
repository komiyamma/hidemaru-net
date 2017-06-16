using System;
using System.Runtime.InteropServices;

using Neo.IronLua;


// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public partial class hmLmDynamicLib
{
    public partial class Hidemaru
    {
        public class Edit
        {
            static Edit()
            {
                SetUnManagedDll();
            }
            /// <summary>
            ///  CursorPos
            /// </summary>
            public static LuaTable CursorPos
            {
                get
                {
                    return GetCursorPos();
                }
            }
            /// <summary>
            ///  [EXPORT] CursorPosFromMousePos
            /// </summary>
            public static LuaTable CursorPosFromMousePos
            {
                get
                {
                    return GetCursorPosFromMousePos();
                }
            }

            // columnやlinenoはエディタ的な座標である。
            private static LuaTable GetCursorPos()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    var t = new LuaTable();
                    t["column"] = -1;
                    t["lineno"] = -1;
                    return t;
                }

                int column = -1;
                int lineno = -1;
                int success = pGetCursorPosUnicode(ref lineno, ref column);
                if (success > 0)
                {
                    LuaTable p = new LuaTable();
                    p["lineno"] = lineno;
                    p["column"] = column;
                    return p;
                }

                var t = new LuaTable();
                t["column"] = -1;
                t["lineno"] = -1;
                return t;
            }

            // columnやlinenoはエディタ的な座標である。
            private static LuaTable GetCursorPosFromMousePos()
            {
                if (version < 873)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed873);
                    var t = new LuaTable();
                    t["column"] = -1;
                    t["lineno"] = -1;
                    return t;
                }

                // この関数が存在しないバージョン
                if (pGetCursorPosUnicodeFromMousePos == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed873);
                    var t = new LuaTable();
                    t["column"] = -1;
                    t["lineno"] = -1;
                    return t;
                }

                int column = -1;
                int lineno = -1;
                pGetCursorPosUnicodeFromMousePos(IntPtr.Zero, ref lineno, ref column);

                LuaTable p = new LuaTable();
                p["lineno"] = lineno;
                p["column"] = column;
                return p;
            }


            /// <summary>
            /// TotalText
            /// </summary>
            public static String TotalText
            {
                get
                {
                    return GetTotalText();
                }
                set
                {
                    SetTotalText(value);
                }
            }

            private static void SetTotalText(String value)
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

                SetTmpVar(value);
                String cmd = ModifyFuncCallByDllType(
                    "begingroupundo;\n" +
                    "selectall;\n" +
                    "insert dllfuncstrw( {0} \"PopStrVar\" );\n" +
                    "endgroupundo;\n"
                );
                Macro.Eval(cmd);
                SetTmpVar(null);
            }


            // 途中でエラーが出るかもしれないので、相応しいUnlockやFreeが出来るように内部管理用
            private enum HGlobalStatus { None, Lock, Unlock, Free };

            // 現在の秀丸の編集中のテキスト全て。元が何の文字コードでも関係なく秀丸がwchar_tのユニコードで返してくれるので、
            // String^型に入れておけば良い。
            public static String GetTotalText()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return "";
                }

                String curstr = "";
                IntPtr hGlobal = pGetTotalTextUnicode();
                HGlobalStatus hgs = HGlobalStatus.None;
                if (hGlobal != null)
                {
                    try
                    {
                        IntPtr ret = GlobalLock(hGlobal);
                        hgs = HGlobalStatus.Lock;
                        curstr = Marshal.PtrToStringUni(ret);
                        GlobalUnlock(hGlobal);
                        hgs = HGlobalStatus.Unlock;
                        GlobalFree(hGlobal);
                        hgs = HGlobalStatus.Free;
                    }
                    catch (Exception e)
                    {
                        OutputDebugStream(e.Message);
                    }
                    finally
                    {
                        switch (hgs)
                        {
                            // ロックだけ成功した
                            case HGlobalStatus.Lock:
                                {
                                    GlobalUnlock(hGlobal);
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // アンロックまで成功した
                            case HGlobalStatus.Unlock:
                                {
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // フリーまで成功した
                            case HGlobalStatus.Free:
                                {
                                    break;
                                }
                        }
                    }
                }
                return curstr;
            }


            /// <summary>
            ///  SelecetdText
            /// </summary>
            public static String SelectedText
            {
                get
                {
                    return GetSelectedText();
                }
                set
                {
                    SetSelectedText(value);
                }
            }

            // 現在の秀丸の選択中のテキスト。元が何の文字コードでも関係なく秀丸がwchar_tのユニコードで返してくれるので、
            // String^型に入れておけば良い。
            public static String GetSelectedText()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return "";
                }

                String curstr = "";
                IntPtr hGlobal = pGetSelectedTextUnicode();
                HGlobalStatus hgs = HGlobalStatus.None;
                if (hGlobal != null)
                {
                    try
                    {
                        IntPtr ret = GlobalLock(hGlobal);
                        hgs = HGlobalStatus.Lock;
                        curstr = Marshal.PtrToStringUni(ret);
                        GlobalUnlock(hGlobal);
                        hgs = HGlobalStatus.Unlock;
                        GlobalFree(hGlobal);
                        hgs = HGlobalStatus.Free;
                    }
                    catch (Exception e)
                    {
                        OutputDebugStream(e.Message);
                    }
                    finally
                    {
                        switch (hgs)
                        {
                            // ロックだけ成功した
                            case HGlobalStatus.Lock:
                                {
                                    GlobalUnlock(hGlobal);
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // アンロックまで成功した
                            case HGlobalStatus.Unlock:
                                {
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // フリーまで成功した
                            case HGlobalStatus.Free:
                                {
                                    break;
                                }
                        }
                    }
                }

                if (curstr == null)
                {
                    curstr = "";
                }
                return curstr;
            }


            private static void SetSelectedText(String value)
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

                SetTmpVar(value);
                String invocate = ModifyFuncCallByDllType("{0}");
                String cmd =
                    "if (selecting) {\n" +
                    "insert dllfuncstrw( " + invocate + " \"PopStrVar\" );\n" +
                    "}\n";
                Macro.Eval(cmd);
                SetTmpVar(null);
            }

            /// <summary>
            /// LineText
            /// </summary>
            public static String LineText
            {
                get
                {
                    return GetLineText();
                }
                set
                {
                    SetLineText(value);
                }
            }

            // 現在の秀丸の編集中のテキストで、カーソルがある行だけのテキスト。
            // 元が何の文字コードでも関係なく秀丸がwchar_tのユニコードで返してくれるので、
            // String^型に入れておけば良い。
            public static String GetLineText()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return "";
                }

                LuaTable p = GetCursorPos();

                String curstr = "";
                IntPtr hGlobal = pGetLineTextUnicode((int)p["lineno"]);
                HGlobalStatus hgs = HGlobalStatus.None;
                if (hGlobal != null)
                {
                    try
                    {
                        IntPtr ret = GlobalLock(hGlobal);
                        hgs = HGlobalStatus.Lock;
                        curstr = Marshal.PtrToStringUni(ret);
                        GlobalUnlock(hGlobal);
                        hgs = HGlobalStatus.Unlock;
                        GlobalFree(hGlobal);
                        hgs = HGlobalStatus.Free;
                    }
                    catch (Exception e)
                    {
                        OutputDebugStream(e.Message);
                    }
                    finally
                    {
                        switch (hgs)
                        {
                            // ロックだけ成功した
                            case HGlobalStatus.Lock:
                                {
                                    GlobalUnlock(hGlobal);
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // アンロックまで成功した
                            case HGlobalStatus.Unlock:
                                {
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // フリーまで成功した
                            case HGlobalStatus.Free:
                                {
                                    break;
                                }
                        }
                    }
                }
                return curstr;
            }

            private static void SetLineText(String value)
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

                SetTmpVar(value);
                var pos = GetCursorPos();
                String cmd = ModifyFuncCallByDllType(
                    "begingroupundo;\n" +
                    "selectline;\n" +
                    "insert dllfuncstrw( {0} \"PopStrVar\" );\n" +
                    "moveto2 " + pos["column"] + ", " + pos["lineno"] + ";\n" +
                    "endgroupundo;\n"
                );
                Macro.Eval(cmd);
                SetTmpVar(null);
            }
        }
    }
}
