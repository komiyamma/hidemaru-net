/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

internal sealed class HmWndHandleSearcher
{
    String strHidemaruClassName;
    IntPtr hCurWndHidemaru = IntPtr.Zero;
    public HmWndHandleSearcher(String strClassName)
    {
        this.strHidemaruClassName = strClassName;
        hCurWndHidemaru = IntPtr.Zero;
    }

    [DllImport("user32.dll", SetLastError = false)]
    static extern IntPtr GetDesktopWindow();

    private IntPtr SearchCurWndHidemaru()
    {
        IntPtr hWndDeskTop = GetDesktopWindow();
        // まず普通のタブモードの秀丸
        if (hCurWndHidemaru == IntPtr.Zero)
        {
            // 内部でhCurWndHidemaruに代入している
            TabOnSearchCurWndHidemaru(hWndDeskTop);
        }

        // 次にタブではないモードの秀丸
        if (hCurWndHidemaru == IntPtr.Zero)
        {
            // 内部でhCurWndHidemaruに代入している
            NoTabSearchCurWndHidemaru(hWndDeskTop);
        }

        // 以上の２つの順番で検索していく必要がある
        return hCurWndHidemaru;
    }

    [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
    static extern int GetClassName(IntPtr hWnd, StringBuilder lpClassName, int nMaxCount);

    private bool IsWndHidemaru32ClassType(IntPtr hWnd)
    {
        if (hWnd == IntPtr.Zero)
        {
            return false;
        }

        StringBuilder strTargetClassName = new StringBuilder(256);

        int nRet = GetClassName(hWnd, strTargetClassName, strTargetClassName.Capacity);

        if (strHidemaruClassName == strTargetClassName.ToString())
        {
            return true;
        }

        return false;
    }

    [DllImport("user32.dll", ExactSpelling = true, CharSet = CharSet.Auto)]
    static extern IntPtr GetParent(IntPtr hWnd);

    [DllImport("user32.dll", SetLastError = true)]
    static extern uint GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

    [DllImport("user32.dll", SetLastError = true)]
    static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, IntPtr szTitle);

    private void TabOnSearchCurWndHidemaru(IntPtr hWnd)
    {
        // すでに発見済み
        if (this.hCurWndHidemaru != IntPtr.Zero) { return; }

        IntPtr hWndParent = GetParent(hWnd);

        // 有効だ
        if (hWndParent != IntPtr.Zero)
        {

            // 自分のプロセスIDと、サーチ対象のプロセスID
            Process currentProcess = Process.GetCurrentProcess();
            int pID1 = currentProcess.Id;

            int pID2 = 0;
            GetWindowThreadProcessId(hWnd, out pID2);

            // 同じなら大きく候補だ
            if (pID1 == pID2)
            {

                // 自分自身の親も指定のクラス名なら、完全に特定した。
                if (IsWndHidemaru32ClassType(hWndParent))
                {
                    this.hCurWndHidemaru = hWnd;
                }
            }
        }

        // 子クラスをなめていく。子クラスはあくまでも「Hidemaru32Class系」。
        // ストア版はちょっと違うのでインスタンス変数になっている。
        IntPtr hWndChild = FindWindowEx(hWnd, IntPtr.Zero, this.strHidemaruClassName, IntPtr.Zero);
        while (hWndChild != IntPtr.Zero)
        {
            TabOnSearchCurWndHidemaru(hWndChild);
            if (this.hCurWndHidemaru != IntPtr.Zero) { break; }

            hWndChild = FindWindowEx(hWnd, hWndChild, this.strHidemaruClassName, IntPtr.Zero);
        }
    }

    private void NoTabSearchCurWndHidemaru(IntPtr hWnd)
    {
        // すでに発見済み
        if (this.hCurWndHidemaru != IntPtr.Zero) { return; }

        IntPtr hWndParent = GetParent(hWnd);

        // 自分は指定のクラス名だ
        if (IsWndHidemaru32ClassType(hWndParent))
        {
            // 自分のプロセスIDと、サーチ対象のプロセスID
            Process currentProcess = Process.GetCurrentProcess();
            int pID1 = currentProcess.Id;

            int pID2 = 0;
            GetWindowThreadProcessId(hWnd, out pID2);

            // 非タブモードなら、同じならそれにそういない
            if (pID1 == pID2)
            {
                this.hCurWndHidemaru = hWnd;
            }
        }

        // 子クラスをなめていく。子クラスはあくまでも「Hidemaru32Class系」。
        // ストア版はちょっと違うのでインスタンス変数になっている。
        IntPtr hWndChild = FindWindowEx(hWnd, IntPtr.Zero, this.strHidemaruClassName, IntPtr.Zero);
        while (hWndChild != IntPtr.Zero)
        {
            NoTabSearchCurWndHidemaru(hWndChild);
            if (this.hCurWndHidemaru != IntPtr.Zero) { break; }

            hWndChild = FindWindowEx(hWnd, hWndChild, this.strHidemaruClassName, IntPtr.Zero);
        }
    }

    [DllImport("user32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern bool IsWindow(IntPtr hWnd);

    public static IntPtr GetCurWndHidemaru(IntPtr hCurWnd)
    {
        // 現状が機能しているならそのまま
        if (IsWindow(hCurWnd))
        {
            return hCurWnd;
        }

        // 元々はいっていなかったら、もしくは、タブモードの切り替え等で機能しなくなった

        IntPtr hWnd = IntPtr.Zero;

        // まず普通の秀丸
        if (hWnd == IntPtr.Zero)
        {
            HmWndHandleSearcher s1 = new HmWndHandleSearcher("Hidemaru32Class");
            hWnd = s1.SearchCurWndHidemaru();
        }

        // 次にストアアプリ版
        if (hWnd == IntPtr.Zero)
        {
            HmWndHandleSearcher s1 = new HmWndHandleSearcher("Hidemaru32Class_Appx");
            hWnd = s1.SearchCurWndHidemaru();
        }

        return hWnd;
    }
}
