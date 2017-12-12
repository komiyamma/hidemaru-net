/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

using Hidemaru;
using System.Collections.Generic;
using System.Diagnostics;

internal partial class HmWebBrowserModeForm : Form
{
    [DllImport("user32.dll")]
    static extern IntPtr GetActiveWindow();

    [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern int GetClassName(IntPtr hWnd, StringBuilder lpClassName, int nMaxCount);

    [DllImport("user32.dll")]
    public static extern IntPtr GetForegroundWindow();

    [DllImport("user32.dll")]
    public static extern int GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);


    private bool IsActiveWindowIsHidemaruMainWindow()
    {
        IntPtr hWnd = GetActiveWindow();
        StringBuilder ClassName = new StringBuilder(256);
        int nRet = GetClassName(hWnd, ClassName, ClassName.Capacity);
        if (ClassName.ToString().Contains("Hidemaru32Class"))
        {
            return true;
        }
        return false;
    }
    private bool IsForegroundWindowIsHidemaruMainWindow()
    {
        IntPtr hWnd = Hm.WindowHandle;
        IntPtr hParent = GetParent(hWnd);
        if (hParent == IntPtr.Zero)
        {
            hParent = hWnd;
        }
        IntPtr hWndForeGround = GetForegroundWindow();
        if (hWndForeGround == hParent)
        {
            return true;
        }
        return false;
    }

    [DllImport("user32.dll")]
    static extern IntPtr WindowFromPoint(System.Drawing.Point p);

    [DllImport("user32.dll")]
    static extern bool GetCursorPos(out Point lpPoint);

    [DllImport("user32.dll", SetLastError = true)]
    static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

    [DllImport("kernel32.dll")]
    static extern uint GetCurrentProcessId();

    private bool IsUnderWindowIsCurrentProcessWindow()
    {

        Point po;
        if (GetCursorPos(out po))
        {
            // マウスの下
            IntPtr hWndUner = WindowFromPoint(po);

            // マウスの下と、自分自身のプロセス比較
            int processID1 = 0;
            int threadID = GetWindowThreadProcessId(hWndUner, out processID1);

            uint processID2 = GetCurrentProcessId();

            if (processID1 == processID2)
            {
                return true;
            }
            // 違う。だが、タブによって合体していて、Hidemaru32Clientの親と、Hidemaru32Clientの子という関係になっているかもしれない。
            else
            {
                // hidemaruhandle(0)の親が、マウスの下のウィンドウなら、それはプロセスは異なるが真とみなす。
                IntPtr hParentUnder = GetParent(hWndUner);

                Process pHidemaru = Process.GetCurrentProcess();
                Process pUnder = Process.GetProcessById(processID1);
                // プロセスの名前は異なれば
                if (pUnder != null && pHidemaru.ProcessName != pUnder.ProcessName)
                {
                    // ここではチェックしない
                    return true;
                }

                // hidemaruhandle(0)の親が、マウスの下のウィンドウなら、それはプロセスは異なるが真とみなす。
                // 親が無いということは、違う
                IntPtr hParentHmWnd = GetParent(Hm.WindowHandle);
                if (hParentHmWnd == IntPtr.Zero)
                {
                    return false;
                }
                // マウスの下ウィンドウのハンドルと、秀丸ハンドルの親のウィンドウが同じということは、タブモードで秀丸本体付近部分にマウスが当たっている
                if (hWndUner == hParentHmWnd)
                {
                    return true;
                }

                return false;
            }
        }
        return false;
    }

    [DllImport("user32.dll", SetLastError = true)]
    static extern bool GetWindowRect(IntPtr hwnd, out RECT lpRect);

    [StructLayout(LayoutKind.Sequential)]
    public struct RECT
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [DllImport("user32.dll", SetLastError = true)]
    static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, IntPtr szTitle);

    [DllImport("user32.dll", SetLastError = true)]
    static extern IntPtr SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

    [DllImport("user32.dll", ExactSpelling = true, CharSet = CharSet.Auto)]
    public static extern IntPtr GetParent(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern IntPtr GetWindow(IntPtr hWnd, uint wCmd);

    // Zオーダー順に収められた秀丸ハンドルのリスト
    List<IntPtr> GetWindowHidemaruHandleList()
    {
        const uint GW_HWNDPREV = 3;
        List<IntPtr> list = new List<IntPtr>();
        IntPtr hCurWnd = Hm.WindowHandle;
        list.Add(hCurWnd);

        // 自分より前方を走査
        IntPtr hTmpWnd = hCurWnd;
        while (true)
        {
            // 次のウィンドウ
            hTmpWnd = GetWindow(hTmpWnd, GW_HWNDPREV);
            if (hTmpWnd == IntPtr.Zero)
            {
                break;
            }

            // タブモードなので親があるハズ。(非タブモードだとそもそも１つしかない)
            IntPtr hParent = GetParent(hTmpWnd);
            if (hParent == IntPtr.Zero)
            {
                break;
            }

            // クラス名で判断
            StringBuilder ClassName = new StringBuilder(256);
            int nRet = GetClassName(hTmpWnd, ClassName, ClassName.Capacity);

            // クラス名にHidemaru32Classが含まれる
            if (ClassName.ToString().Contains("Hidemaru32Class")) {

                list.Insert(0, hTmpWnd);
            }
        }

        // 一旦また自身のウィンドウハンドルにリセットして…
        hTmpWnd = hCurWnd;

        // 自分より後方を走査
        const uint GW_HWNDNEXT = 2;
        while (true)
        {
            // 次のウィンドウ
            hTmpWnd = GetWindow(hTmpWnd, GW_HWNDNEXT);
            if (hTmpWnd == IntPtr.Zero)
            {
                break;
            }

            // タブモードななので親があるハズ。(非タブモードだとそもそも１つしかない)
            IntPtr hParent = GetParent(hTmpWnd);
            if (hParent == IntPtr.Zero)
            {
                break;
            }

            // クラス名で判断
            StringBuilder ClassName = new StringBuilder(256);
            int nRet = GetClassName(hTmpWnd, ClassName, ClassName.Capacity);

            // クラス名にHidemaru32Classが含まれる
            if (ClassName.ToString().Contains("Hidemaru32Class"))
            {

                list.Add(hTmpWnd);
            }
        }

        return list;
    }

    [DllImport("user32.dll")]
    public static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

    [DllImport("user32.dll")]
    public static extern int GetWindowLong(IntPtr hWnd, int nIndex);

    private void ChangeSetParent(IntPtr hWndParent)
    {
        const int GWL_STYLE = -16;
        const int WS_CHILD = 0x40000000;

        IntPtr guestHandle = this.Handle;
        SetWindowLong(guestHandle, GWL_STYLE, GetWindowLong(guestHandle, GWL_STYLE) | WS_CHILD);
        SetParent(guestHandle, hWndParent);
    }
}
