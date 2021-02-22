/*
 * HmNetPInvoke ver 1.001
 * Copyright (C) 2021 Akitsugu Komiyama
 * under the MIT License
 **/

using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace HmNetPInvoke
{
#if BUILD_DLL
    public partial class Hm
#else
    internal partial class Hm
#endif
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

        private const int filePathMaxLength = 512;

        private static string GetHidemaruExeFullPath()
        {
            var sb = new System.Text.StringBuilder(filePathMaxLength);
            GetModuleFileName(IntPtr.Zero, sb, filePathMaxLength);
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

        public partial class Edit
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
                            new InvalidOperationException("Hidemaru_GetTotalTextUnicode_Exception");
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
                                new InvalidOperationException("Hidemaru_GetSelectedTextUnicode_Exception");
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

                        ICursorPos pos = CursorPos;
                        if (pos.LineNo < 0 || pos.Column < 0)
                        {
                            return lineText;
                        }

                        try
                        {
                            IntPtr hGlobal = Hidemaru_GetLineTextUnicode(pos.LineNo);
                            if (hGlobal == IntPtr.Zero)
                            {
                                new InvalidOperationException("Hidemaru_GetLineTextUnicode_Exception");
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
            /// CursorPos の返り値のインターフェイス
            /// </summary>
            /// <returns>(LineNo, Column)</returns>
            public interface ICursorPos
            {
                int LineNo { get; }
                int Column { get; }
            }

            private struct TCursurPos : ICursorPos
            {
                public int Column { get; set; }
                public int LineNo { get; set; }
            }

            /// <summary>
            /// MousePos の返り値のインターフェイス
            /// </summary>
            /// <returns>(LineNo, Column, X, Y)</returns>
            public interface IMousePos
            {
                int LineNo { get; }
                int Column { get; }
                int X { get; }
                int Y { get; }
            }

            private struct TMousePos : IMousePos
            {
                public int LineNo { get; set; }
                public int Column { get; set; }
                public int X { get; set; }
                public int Y { get; set; }
            }

            /// <summary>
            /// ユニコードのエディタ的な換算でのカーソルの位置を返す
            /// </summary>
            /// <returns>(LineNo, Column)</returns>
            public static ICursorPos CursorPos
            {
                get
                {
                    int lineno = -1;
                    int column = -1;
                    bool success = Hidemaru_GetCursorPosUnicode(out lineno, out column);
                    if (success)
                    {
                        TCursurPos pos = new TCursurPos();
                        pos.LineNo = lineno;
                        pos.Column = column;
                        return pos;
                    }
                    else
                    {
                        TCursurPos pos = new TCursurPos();
                        pos.LineNo = -1;
                        pos.Column = -1;
                        return pos;
                    }

                }
            }

            /// <summary>
            /// ユニコードのエディタ的な換算でのマウスの位置に対応するカーソルの位置を返す
            /// </summary>
            /// <returns>(LineNo, Column, X, Y)</returns>
            public static IMousePos MousePos
            {
                get
                {
                    POINT lpPoint;
                    bool success_1 = GetCursorPos(out lpPoint);

                    TMousePos pos = new TMousePos
                    {
                        LineNo = -1,
                        Column = -1,
                        X = -1,
                        Y = -1,
                    };

                    if (!success_1)
                    {
                        return pos;
                    }

                    int column = -1;
                    int lineno = -1;
                    bool success_2 = Hidemaru_GetCursorPosUnicodeFromMousePos(IntPtr.Zero, out lineno, out column);
                    if (!success_2)
                    {
                        return pos;
                    }

                    pos.LineNo = lineno;
                    pos.Column = column;
                    pos.X = lpPoint.X;
                    pos.Y = lpPoint.Y;
                    return pos;
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

                        StringBuilder sb = new StringBuilder(filePathMaxLength); // まぁこんくらいでさすがに十分なんじゃないの...
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

        public class Macro
        {
            /// <summary>
            /// マクロを実行中か否かを判定する
            /// </summary>
            /// <returns>実行中ならtrue, そうでなければfalse</returns>

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

            /// <summary>
            /// マクロをプログラム内から実行した際の返り値のインターフェイス
            /// </summary>
            /// <returns>(Result, Message, Error)</returns>
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

            /// <summary>
            /// 現在のマクロ実行中に、プログラム中で、マクロを文字列で実行。
            /// マクロ実行中のみ実行可能なメソッド。
            /// </summary>
            /// <returns>(Result, Message, Error)</returns>

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
                    Exception e = new InvalidOperationException("Hidemaru_Macro_Eval_Exception");
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
                /// <summary>
                /// マクロを実行していない時に、プログラム中で、マクロファイルを与えて新たなマクロを実行。
                /// マクロを実行していない時のみ実行可能なメソッド。
                /// </summary>
                /// <returns>(Result, Message, Error)</returns>

                public static IResult File(string filepath)
                {
                    TResult result;
                    if (IsExecuting)
                    {
                        Exception e = new InvalidOperationException("Hidemaru_Macro_IsExecuting_Exception");
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
                        Exception e = new InvalidOperationException("Hidemaru_Macro_Eval_Exception");
                        result = new TResult(0, sbRet.ToString(), e);
                    }
                    return result;
                }

                /// <summary>
                /// マクロを実行していない時に、プログラム中で、文字列で新たなマクロを実行。
                /// マクロを実行していない時のみ実行可能なメソッド。
                /// </summary>
                /// <returns>(Result, Message, Error)</returns>
                public static IResult Eval(string expression)
                {
                    TResult result;
                    if (IsExecuting)
                    {
                        Exception e = new InvalidOperationException("Hidemaru_Macro_IsExecuting_Exception");
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
                        Exception e = new InvalidOperationException("Hidemaru_Macro_Eval_Exception");
                        result = new TResult(0, sbRet.ToString(), e);
                    }
                    return result;
                }
            }
        }
    }
}

namespace HmNetPInvoke
{

#if BUILD_DLL
    public partial class Hm
#else
    internal partial class Hm
#endif
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

namespace HmNetPInvoke
{
#if BUILD_DLL
    public partial class Hm
#else
    internal partial class Hm
#endif
    {
        [DllImport("kernel32.dll")]
        private extern static uint GetModuleFileName(IntPtr hModule, System.Text.StringBuilder lpFilename, int nSize);

        [DllImport("kernel32.dll", SetLastError = true)]
        private extern static IntPtr GlobalLock(IntPtr hMem);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]

        private extern static bool GlobalUnlock(IntPtr hMem);
        [DllImport("kernel32.dll", SetLastError = true)]
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
        private static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        private static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
        private static extern bool SendMessage(IntPtr hWnd, uint Msg, StringBuilder wParam, StringBuilder lParam);

    }
}