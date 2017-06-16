using System;
using System.Runtime.InteropServices;

// ★秀丸クラス
public partial class hmV8DynamicLib
{
    public partial class Hidemaru
    {

        // 座標型。Point型では、System.Drawingを読み込まないとダメなので負荷がある。また、x, yは秀丸に別値として存在するので、
        // あくまでも、マクロのcolumnとlinenoと一致しているという主張。なお、x, yはワープロ的な座標を拾ってくる。
        // columnやlinenoはエディタ的な座標である。
        public struct HmCursurPos
        {
            private int m_lineno;
            private int m_column;
            public HmCursurPos(int lineno, int column)
            {
                this.m_lineno = lineno;
                this.m_column = column;
            }
            public int column { get { return m_column; } }
            public int lineno { get { return m_lineno; } }
        }

        public class Edit
        {
            static Edit()
            {
                SetUnManagedDll();
            }
            /// <summary>
            ///  CursorPos
            /// </summary>
            public static HmCursurPos CursorPos
            {
                get
                {
                    return GetCursorPos();
                }
            }
            /// <summary>
            ///  [EXPORT] CursorPosFromMousePos
            /// </summary>
            public static HmCursurPos CursorPosFromMousePos
            {
                get
                {
                    return GetCursorPosFromMousePos();
                }
            }

            // columnやlinenoはエディタ的な座標である。
            private static HmCursurPos GetCursorPos()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return new HmCursurPos(-1, -1);
                }

                int column = -1;
                int lineno = -1;
                int success = pGetCursorPosUnicode(ref lineno, ref column);
                if (success > 0)
                {
                    HmCursurPos p = new HmCursurPos(lineno, column);
                    return p;
                }

                return new HmCursurPos(-1, -1);
            }
            // columnやlinenoはエディタ的な座標である。
            private static HmCursurPos GetCursorPosFromMousePos()
            {
                if (_ver < 873)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed873);
                    return new HmCursurPos(-1, -1);
                }

                // この関数が存在しないバージョン
                if (pGetCursorPosUnicodeFromMousePos == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed873);
                    return new HmCursurPos(-1, -1);
                }

                int column = -1;
                int lineno = -1;
                pGetCursorPosUnicodeFromMousePos(IntPtr.Zero, ref lineno, ref column);
                HmCursurPos p = new HmCursurPos(lineno, column);
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

            // 途中でエラーが出るかもしれないので、相応しいUnlockやFreeが出来るように内部管理用
            private enum HGlobalStatus { None, Lock, Unlock, Free };

            // 現在の秀丸の編集中のテキスト全て。元が何の文字コードでも関係なく秀丸がwchar_tのユニコードで返してくれるので、
            // String^型に入れておけば良い。
            private static String GetTotalText()
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
                Macro._Eval(cmd);
                SetTmpVar(null);
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
            private static String GetSelectedText()
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
                Macro._Eval(cmd);
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
            private static String GetLineText()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return "";
                }

                HmCursurPos p = GetCursorPos();

                String curstr = "";
                IntPtr hGlobal = pGetLineTextUnicode(p.lineno);
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
                    "moveto2 " + pos.column + ", " + pos.lineno + ";\n" +
                    "endgroupundo;\n"
                );
                Macro._Eval(cmd);
                SetTmpVar(null);
            }
        }
    }
}
