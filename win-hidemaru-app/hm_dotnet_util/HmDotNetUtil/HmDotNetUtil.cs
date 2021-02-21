using System;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;


namespace test {
    using HmDotNetUtil;

    public class abc
    {
        static abc()
        {
            String str = Hm.Edit.TotalText;
            var pos = Hm.Macro.Eval(@"message(""abc"");");
        }
    }
}


namespace HmDotNetUtil
{
    internal partial class Hm
    {
        private static double _version = 0;
        /// <summary>
        /// 秀丸バージョンの取得
        /// </summary>
        /// <returns>秀丸バージョン</returns>
        public static double Version
        {
            get
            {
                if (_version == 0)
                {
                    string hidemaru_fullpath = GetHidemaruExeFullPath();
                    System.Diagnostics.FileVersionInfo vi = System.Diagnostics.FileVersionInfo.GetVersionInfo(hidemaru_fullpath);
                    _version = 100 * vi.FileMajorPart + 10 * vi.FileMinorPart + 1 * vi.FileBuildPart + 0.01 * vi.FilePrivatePart;
                }

                return _version;
            }
        }

        private static string GetHidemaruExeFullPath()
        {
            const int PATH_MAX = 512;
            var sb = new System.Text.StringBuilder(PATH_MAX);
            GetModuleFileName(IntPtr.Zero, sb, PATH_MAX);
            string hidemaru_fullpath = sb.ToString();
            return hidemaru_fullpath;
        }

        /// <summary>
        /// 呼ばれたプロセスの現在の秀丸エディタのウィンドウハンドルを返します。
        /// </summary>
        /// <returns>現在の秀丸エディタのウィンドウハンドル</returns>
        public static IntPtr WindowHandle
        {
            get
            {
                return Hidemaru_GetCurrentWindowHandle();
            }
        }

        internal partial class Edit
        {
            /// <summary>
            /// 現在アクティブな編集領域のテキスト全体を返す。
            /// </summary>
            /// <returns>編集領域のテキスト全体</returns>

            public static string TotalText
            {
                get
                {
                    string totalText = "";
                    try
                    {
                        IntPtr hGlobal = Hidemaru_GetTotalTextUnicode();
                        if (hGlobal == IntPtr.Zero)
                        {
                            new InvalidOperationException("Hidemaru_GetTotalTextUnicode");
                        }

                        var pwsz = GlobalLock(hGlobal);
                        if (pwsz != IntPtr.Zero)
                        {
                            totalText = Marshal.PtrToStringUni(pwsz);
                            GlobalUnlock(hGlobal);
                        }
                        GlobalFree(hGlobal);
                    }
                    catch (Exception)
                    {
                        throw;
                    }

                    return totalText;
                }
            }

            /// <summary>
            /// 現在、単純選択している場合、その選択中のテキスト内容を返す。
            /// </summary>
            /// <returns>選択中のテキスト内容</returns>
            public static string SelectedText
            {
                get
                {
                    {
                        string selectedText = "";
                        try
                        {
                            IntPtr hGlobal = Hidemaru_GetSelectedTextUnicode();
                            if (hGlobal == IntPtr.Zero)
                            {
                                new InvalidOperationException("Hidemaru_GetSelectedTextUnicode");
                            }

                            var pwsz = GlobalLock(hGlobal);
                            if (pwsz != IntPtr.Zero)
                            {
                                selectedText = Marshal.PtrToStringUni(pwsz);
                                GlobalUnlock(hGlobal);
                            }
                            GlobalFree(hGlobal);
                        }
                        catch (Exception)
                        {
                            throw;
                        }

                        return selectedText;
                    }
                }
            }

            /// <summary>
            /// 現在、カーソルがある行(エディタ的)のテキスト内容を返す。
            /// </summary>
            /// <returns>選択中のテキスト内容</returns>
            public static string LineText
            {
                get
                {
                    {
                        string lineText = "";

                        (int lineno, int column) = CursorPos;
                        if (lineno < 0 || column < 0)
                        {
                            return lineText;
                        }

                        try
                        {
                            IntPtr hGlobal = Hidemaru_GetLineTextUnicode((int)lineno);
                            if (hGlobal == IntPtr.Zero)
                            {
                                new InvalidOperationException("Hidemaru_GetLineTextUnicode");
                            }

                            var pwsz = GlobalLock(hGlobal);
                            if (pwsz != IntPtr.Zero)
                            {
                                lineText = Marshal.PtrToStringUni(pwsz);
                                GlobalUnlock(hGlobal);
                            }
                            GlobalFree(hGlobal);
                        }
                        catch (Exception)
                        {
                            throw;
                        }

                        return lineText;
                    }
                }
            }

            /// <summary>
            /// ユニコードのエディタ的な換算でのカーソルの位置を返す
            /// </summary>
            /// <returns>(LineNo, Column)</returns>
            public static (int LineNo, int Column) CursorPos
            {
                get
                {
                    int lineno = -1;
                    int column = -1;
                    bool success = Hidemaru_GetCursorPosUnicode(out lineno, out column);
                    if (success)
                    {
                        return (lineno, column);
                    }
                    else
                    {
                        return (-1, -1);
                    }

                }
            }

            /// <summary>
            /// ユニコードのエディタ的な換算でのマウスの位置に対応するカーソルの位置を返す
            /// </summary>
            /// <returns>(LineNo, Column, X, Y)</returns>
            public static (int LineNo, int Column, int X, int Y) MousePos
            {
                get
                {
                    POINT lpPoint;
                    bool success_1 = GetCursorPos(out lpPoint);
                    if (!success_1)
                    {
                        return (-1, -1, -1, -1);
                    }

                    int column = -1;
                    int lineno = -1;
                    bool success_2 = Hidemaru_GetCursorPosUnicodeFromMousePos(IntPtr.Zero, out lineno, out column);
                    if (!success_2)
                    {
                        return (-1, -1, -1, -1);
                    }

                    return (column, lineno, lpPoint.X, lpPoint.Y);
                }
            }

            /// <summary>
            /// 現在開いているファイル名のフルパスを返す、無題テキストであれば、nullを返す。
            /// </summary>
            /// <returns>ファイル名のフルパス、もしくは null</returns>

            public static string FilePath
            {
                get
                {
                    IntPtr hWndHidemaru = WindowHandle;
                    if (hWndHidemaru != IntPtr.Zero)
                    {
                        const int WM_USER = 0x400;
                        const int WM_HIDEMARUINFO = WM_USER + 181;
                        const int HIDEMARUINFO_GETFILEFULLPATH = 4;

                        StringBuilder sb = new StringBuilder(512); // まぁこんくらいでさすがに十分なんじゃないの...
                        bool cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, new IntPtr(HIDEMARUINFO_GETFILEFULLPATH), sb);
                        String filename = sb.ToString();
                        if (String.IsNullOrEmpty(filename))
                        {
                            return null;
                        }
                        else
                        {
                            return filename;
                        }
                    }
                    return null;
                }
            }
        }

        internal class Macro
        {
            public static bool IsExecuting
            {
                get
                {
                    const int WM_USER = 0x400;
                    const int WM_ISMACROEXECUTING = WM_USER + 167;

                    IntPtr hWndHidemaru = WindowHandle;
                    if (hWndHidemaru != IntPtr.Zero)
                    {
                        bool cwch = SendMessage(hWndHidemaru, WM_ISMACROEXECUTING, IntPtr.Zero, IntPtr.Zero);
                        return cwch;
                    }

                    return false;
                }
            }

            public interface IResult
            {
                int Result { get; }
                String Message { get; }
                Exception Error { get; }
            }

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

            public static IResult Eval(String expression)
            {
                int success = 0;
                try
                {
                    success = Hidemaru_EvalMacro(expression);
                }
                catch (Exception)
                {
                    throw;
                }
                if (success == 0)
                {
                    Exception e = new InvalidOperationException("HidemaruMacroEvalException");
                    TResult result = new TResult(0, "", e);
                    return result;
                }
                else
                {
                    TResult result = new TResult(success, "", null);
                    return result;
                }

            }

            public class Exec
            {
                public static IResult File(string filepath)
                {
                    TResult result;
                    if (IsExecuting)
                    {
                        Exception e = new InvalidOperationException("HidemaruMacroIsExecutingException");
                        result = new TResult(-1, "", e);
                        return result;
                    }
                    if (!System.IO.File.Exists(filepath))
                    {
                        Exception e = new FileNotFoundException(filepath);
                        result = new TResult(-1, "", e);
                        return result;
                    }

                    const int WM_USER = 0x400;
                    const int WM_REMOTE_EXECMACRO_FILE = WM_USER + 271;
                    IntPtr hWndHidemaru = WindowHandle;

                    StringBuilder sbFileName = new StringBuilder(filepath);
                    StringBuilder sbRet = new StringBuilder("\x0f0f", 0x0f0f + 1); // 最初の値は帰り値のバッファー
                    bool cwch = SendMessage(hWndHidemaru, WM_REMOTE_EXECMACRO_FILE, sbRet, sbFileName);
                    if (cwch)
                    {
                        result = new TResult(1, sbRet.ToString(), null);
                    }
                    else
                    {
                        Exception e = new InvalidOperationException("HidemaruMacroEvalException");
                        result = new TResult(0, sbRet.ToString(), e);
                    }
                    return result;
                }

                public static IResult Eval(string expression)
                {
                    TResult result;
                    if (IsExecuting)
                    {
                        Exception e = new InvalidOperationException("HidemaruMacroIsExecutingException");
                        result = new TResult(-1, "", e);
                        return result;
                    }

                    const int WM_USER = 0x400;
                    const int WM_REMOTE_EXECMACRO_MEMORY = WM_USER + 272;
                    IntPtr hWndHidemaru = WindowHandle;

                    StringBuilder sbExpression = new StringBuilder(expression);
                    StringBuilder sbRet = new StringBuilder("\x0f0f", 0x0f0f + 1); // 最初の値は帰り値のバッファー
                    bool cwch = SendMessage(hWndHidemaru, WM_REMOTE_EXECMACRO_MEMORY, sbRet, sbExpression);
                    if (cwch)
                    {
                        result = new TResult(1, sbRet.ToString(), null);
                    }
                    else
                    {
                        Exception e = new InvalidOperationException("HidemaruMacroEvalException");
                        result = new TResult(0, sbRet.ToString(), e);
                    }
                    return result;
                }

            }
        }

    }
}

namespace HmDotNetUtil
{

    internal partial class Hm
    {
        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        private extern static IntPtr Hidemaru_GetTotalTextUnicode();

        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        private extern static IntPtr Hidemaru_GetSelectedTextUnicode();

        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        private extern static IntPtr Hidemaru_GetLineTextUnicode(int nLineNo);

        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        private extern static IntPtr Hidemaru_GetCurrentWindowHandle();

        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private extern static bool Hidemaru_GetCursorPosUnicode(out int lineno, out int column);

        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private extern static bool Hidemaru_GetCursorPosUnicodeFromMousePos(IntPtr ppt, out int lineno, out int column);

        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        private extern static int Hidemaru_EvalMacro([MarshalAs(UnmanagedType.LPWStr)] String pwsz);
    }
}


namespace HmDotNetUtil
{
    internal partial class Hm
    {
        [DllImport("kernel32.dll")]
        private extern static uint GetModuleFileName(IntPtr hModule, System.Text.StringBuilder lpFilename, int nSize);

        [DllImport("kernel32.dll")]
        private extern static IntPtr GlobalLock(IntPtr hMem);

        [DllImport("kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]

        private extern static bool GlobalUnlock(IntPtr hMem);
        [DllImport("kernel32.dll")]
        private extern static IntPtr GlobalFree(IntPtr hMem);

        [StructLayout(LayoutKind.Sequential)]
        private struct POINT
        {
            public int X;
            public int Y;
        }
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private extern static bool GetCursorPos(out POINT lpPoint);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        public static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        public static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        public static extern bool SendMessage(IntPtr hWnd, uint Msg, StringBuilder wParam, StringBuilder lParam);

    }
}

