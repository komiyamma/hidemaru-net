using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;


namespace HmDotNetUtil
{

    public class abc
    {
        static abc()
        {
            String str = Hm.Edit.TotalText;
            var pos = Hm.Edit.CursorPos;
            if (pos.LineNo > 0)
            {

            }
        }
    }
    public partial class Hm
    {
        private static Decimal _version = 0;
        /// <summary>
        /// 秀丸バージョンの取得
        /// </summary>
        /// <returns>秀丸バージョン。Decimal型なので、判定の浮動小数を　if (Hm.Version > (Decimal)8.98) {...} といったようにキャストすること</returns>
        public static Decimal Version
        {
            get
            {

                if (_version == 0)
                {
                    System.Diagnostics.FileVersionInfo vi = System.Diagnostics.FileVersionInfo.GetVersionInfo(Application.ExecutablePath);
                    _version = (Decimal)100 * vi.FileMajorPart + (Decimal)10 * vi.FileMinorPart + (Decimal)1 * vi.FileBuildPart + (Decimal)0.01 * vi.FilePrivatePart;
                }

                return _version;
            }
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

            public static (int LineNo, int Column) CursorPos
            {
                get
                {
                    int lineno = 0;
                    int column = 0;
                    bool success = Hidemaru_GetCursorPosUnicode(out lineno, out column);
                    if (success)
                    {
                        return (lineno, column);
                    } else
                    {
                        return (-1, -1);
                    }

                }
            }

            public static (int LineNo, int Column, int X, int Y) MousePos
            {
                get
                {
                    return (-1, -1, -1, -1);
                }
            }
        }
    }
}

namespace HmDotNetUtil
{
    partial class Hm
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
    }
}


namespace HmDotNetUtil
{
    partial class Hm
    {
        [DllImport("kernel32.dll")]
        private extern static IntPtr GlobalLock(IntPtr hMem);
        [DllImport("kernel32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        private extern static bool GlobalUnlock(IntPtr hMem);
        [DllImport("kernel32.dll")]
        private extern static IntPtr GlobalFree(IntPtr hMem);
    }
}

