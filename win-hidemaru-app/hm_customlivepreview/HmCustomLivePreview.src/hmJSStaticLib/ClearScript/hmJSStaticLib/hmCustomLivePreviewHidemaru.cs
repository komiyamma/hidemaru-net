using System;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Collections.Generic;
using Microsoft.ClearScript;
using Microsoft.ClearScript.Util;


// アンマネージドライブラリの遅延での読み込み。C++のLoadLibraryと同じことをするため
// これをする理由は、このhmPyとHideamru.exeが異なるディレクトリに存在する可能性があるため、
// C#風のDllImportは成立しないからだ。
internal class UnManagedDll : IDisposable
{
    [DllImport("kernel32")]
    static extern IntPtr LoadLibrary(string lpFileName);
    [DllImport("kernel32")]
    static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);
    [DllImport("kernel32")]
    static extern bool FreeLibrary(IntPtr hModule);

    IntPtr moduleHandle;

    public UnManagedDll(string lpFileName)
    {
        moduleHandle = LoadLibrary(lpFileName);
    }

    public IntPtr ModuleHandle
    {
        get
        {
            return moduleHandle;
        }
    }

    public T GetProcDelegate<T>(string method) where T : class
    {
        IntPtr methodHandle = GetProcAddress(moduleHandle, method);
        T r = Marshal.GetDelegateForFunctionPointer(methodHandle, typeof(T)) as T;
        return r;
    }

    public void Dispose()
    {
        FreeLibrary(moduleHandle);
    }
}



// ファイルをフルパスで指定して、アセンブリをロードできるようにする
namespace Microsoft.ClearScript
{
    public partial class HostTypeCollection : PropertyBag
    {
        /// <summary>
        /// Adds types from an assembly to a host type collection. The assembly is specified by name.
        /// </summary>
        /// <param name="assemblyName">The name of the assembly that contains the types to add.</param>
        public void AddAssembly(string assemblyName)
        {
            // そもそもターゲットとなっている場所に存在する(多分フルパスで指定している、など)
            if (System.IO.File.Exists(assemblyName))
            {
                AddAssembly(Assembly.LoadFile(assemblyName));
            }
            else
            {
                MiscHelpers.VerifyNonBlankArgument(assemblyName, "assemblyName", "Invalid assembly name");
                AddAssembly(Assembly.Load(AssemblyTable.GetFullAssemblyName(assemblyName)));
            }
        }
    }
}



// ★秀丸クラス
public partial class HmCustomLivePreviewDynamicLib
{
    public class Hidemaru
    {
        public Hidemaru()
        {
            System.Diagnostics.FileVersionInfo vi = System.Diagnostics.FileVersionInfo.GetVersionInfo(strExecuteFullpath);
            _ver = 100 * vi.FileMajorPart + 10 * vi.FileMinorPart + 1 * vi.FileBuildPart + 0.01 * vi.FilePrivatePart;
        }

        public class ErrorMsg
        {
            public const String MethodNeed866 = "このメソッドは秀丸エディタ v8.66以降で利用可能です。";
            public static readonly String NoDllBindHandle866 = strDllFullPath + "をloaddllした際の束縛変数の値を特定できません";
        }

        // 秀丸本体から出ている関数群
        delegate IntPtr TGetTotalTextUnicode();
        delegate IntPtr TGetLineTextUnicode(int nLineNo);
        delegate IntPtr TGetSelectedTextUnicode();
        delegate int TGetCursorPosUnicode(ref int pnLineNo, ref int pnColumn);
        delegate int TEvalMacro([MarshalAs(UnmanagedType.LPWStr)] String pwsz);
        delegate int TCheckQueueStatus();

        // 秀丸本体から出ている関数群
        static TGetTotalTextUnicode pGetTotalTextUnicode;
        static TGetLineTextUnicode pGetLineTextUnicode;
        static TGetSelectedTextUnicode pGetSelectedTextUnicode;
        static TGetCursorPosUnicode pGetCursorPosUnicode;
        static TEvalMacro pEvalMacro;
        static TCheckQueueStatus pCheckQueueStatus;

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GlobalLock(IntPtr hMem);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern int GlobalUnlock(IntPtr hMem);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr GlobalFree(IntPtr hMem);

        // debuginfo関数
        public static void debuginfo(params Object[] expressions)
        {
            List<String> list = new List<String>();
            foreach (var exp in expressions)
            {
                // V8エンジンのオブジェクトであれば、そのまま出しても意味が無いので…
                if (exp.GetType().Name == "WindowsScriptItem")
                {
                    dynamic dexp = exp;

                    // JSON.stringifyで変換できるはずだ
                    String strify = "";
                    try
                    {
                        strify = engine.Script.JSON.stringify(dexp);
                        list.Add(strify);
                    }
                    catch (Exception)
                    {

                    }

                    // JSON.stringfyで空っぽだったようだ。
                    if (strify == String.Empty)
                    {
                        try
                        {
                            // ECMAScriptのtoString()で変換出来るはずだ…
                            list.Add(dexp.toString());
                        }
                        catch (Exception)
                        {
                            // 変換できなかったら、とりあえず、しょうがないので.NETのToStringで。多分意味はない。
                            list.Add(exp.ToString());
                        }
                    }
                }

                // V8オブジェクトでないなら、普通にToString
                else
                {
                    list.Add(exp.ToString());
                }
            }

            String joind = String.Join(" ", list);
            OutputDebugStream(joind);
        }

        // バージョン。hm.versionのため。読み取り専用
        static double _ver;
        public static double version
        {
            get { return _ver; }
        }

        // 秀丸本体のexeを指すモジュールハンドル
        static UnManagedDll hmExeHandle;

        // 秀丸本体のExport関数を使えるようにポインタ設定。
        static void SetUnManagedDll()
        {
            // OutputDebugStream(version.ToString());
            if (version >= 866) {

                // 初めての代入のみ
                if (hmExeHandle == null)
                {
                    hmExeHandle = new UnManagedDll(strExecuteFullpath);

                    pGetTotalTextUnicode = hmExeHandle.GetProcDelegate<TGetTotalTextUnicode>("Hidemaru_GetTotalTextUnicode");
                    pGetLineTextUnicode = hmExeHandle.GetProcDelegate<TGetLineTextUnicode>("Hidemaru_GetLineTextUnicode");
                    pGetSelectedTextUnicode = hmExeHandle.GetProcDelegate<TGetSelectedTextUnicode>("Hidemaru_GetSelectedTextUnicode");
                    pGetCursorPosUnicode = hmExeHandle.GetProcDelegate<TGetCursorPosUnicode>("Hidemaru_GetCursorPosUnicode");
                    pEvalMacro = hmExeHandle.GetProcDelegate<TEvalMacro>("Hidemaru_EvalMacro");
                    pCheckQueueStatus = hmExeHandle.GetProcDelegate<TCheckQueueStatus>("Hidemaru_CheckQueueStatus");
                }
            }
        }

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

            // columnやlinenoはエディタ的な座標である。
            private static HmCursurPos GetCursorPos()
            {
                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return new HmCursurPos(1, 0);
                }

                int column = -1;
                int lineno = -1;
                pGetCursorPosUnicode(ref lineno, ref column);
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



        public class Macro
        {
            static Macro()
            {
                SetUnManagedDll();

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
            public static int Eval(Object here_document)
            {
                // 文字列で書いているようであれば、普通のEval代わりに使っている
                if (here_document.GetType().Name == "String")
                {
                    return _Eval( (String)here_document );
                }

                if (version < 866)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed866);
                    return 0;
                }

                // OutputDebugStream(here_document.GetType().Name);
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
                return ret;
            }

            // マクロ文字列の実行。複数行を一気に実行可能
            public static Object Var(String var_name, Object value = null) {
                // 読み取りであれば…
                if (value == null) {
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
                } else {
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
