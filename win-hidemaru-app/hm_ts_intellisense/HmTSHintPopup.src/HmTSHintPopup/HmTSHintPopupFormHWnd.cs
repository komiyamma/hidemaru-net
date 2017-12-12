/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

 using System;
using System.Collections.Generic;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

using Hidemaru;

partial class HmTSHintPopupForm : Form
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
            IntPtr hWnd = WindowFromPoint(po);

            int processID1 = 0;
            int threadID = GetWindowThreadProcessId(hWnd, out processID1);

            uint processID2 = GetCurrentProcessId();

            if (processID1 == processID2)
            {
                return true;
            }
        }
        return false;
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

}
