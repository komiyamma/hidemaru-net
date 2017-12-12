/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Runtime.InteropServices;
using System.Text;

class HidemaruWindowInfo
{
    private static class Win32
    {

        // クラス名取得
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern int GetClassName(IntPtr hWnd, StringBuilder lpClassName, int nMaxCount);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool IsIconic(IntPtr hWnd);

        [DllImport("user32.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern IntPtr GetForegroundWindow();
        // ウィンドウハンドル取得
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        // EnumChildWindowsのための、関数デレゲート
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        [return: MarshalAs(UnmanagedType.Bool)]
        internal delegate bool EnumWindowsProcDelegate(IntPtr windowHandle, IntPtr lParam);

        // EnumChildWindowsのための、関数デレゲート
        [DllImport("user32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool EnumChildWindows(IntPtr handle, [MarshalAs(UnmanagedType.FunctionPtr)] EnumWindowsProcDelegate enumProc, IntPtr lParam);
    }

    // マネージド関数⇒EnumChildWindowsの引数に利用可能なデレゲートへ
    static Win32.EnumWindowsProcDelegate _fChildProc = new Win32.EnumWindowsProcDelegate(EnumChildWindowsProc);

    // EnumChildWindowsで毎回呼ばれる実体
    static bool EnumChildWindowsProc(IntPtr handle, IntPtr lParam)
    {
        //ウィンドウのクラス名を取得する
        StringBuilder csb = new StringBuilder(256);
        Win32.GetClassName(handle, csb, csb.Capacity);
        string classname = csb.ToString();

        if (classname == "HM32CLIENT")
        {
            bResultHidemaruWindow = true;
            return false;
        }
        return true;
    }

    // 秀丸のウィンドウがあるかどうか。常住秀丸は関係ない。通常状態やアイコン化を含めて、
    public static bool bResultHidemaruWindow;
    public static bool IsFindWindow()
    {
        bResultHidemaruWindow = false;

        // 秀丸がフォアグラウンドになった瞬間は、下の全プロセスのサーチ対象から一瞬外れることがあるみたいなので、そのパッチ的な対応
        {
            IntPtr wnd = Win32.GetForegroundWindow();
            StringBuilder csb = new StringBuilder(256);
            Win32.GetClassName(wnd, csb, csb.Capacity);
            string classname = csb.ToString();
            if (classname == "Hidemaru32Class" || classname == "Hidemaru32Class_Appx")
            {
                return true;
            }
        }

        // 秀丸がフォアグラウンドではない時はこちら。
        // 「hidemaru.exe」というファイル名は変更しているかもしれないので、クラス名で。
        foreach (System.Diagnostics.Process p in System.Diagnostics.Process.GetProcesses())
        {
            StringBuilder csb = new StringBuilder(256);
            Win32.GetClassName(p.MainWindowHandle, csb, csb.Capacity);
            string classname = csb.ToString();

            if (classname == "Hidemaru32Class" || classname == "Hidemaru32Class_Appx")
            {
                Win32.EnumChildWindows(p.MainWindowHandle, _fChildProc, default(IntPtr));
            }
        }

        return bResultHidemaruWindow;
    }
}
