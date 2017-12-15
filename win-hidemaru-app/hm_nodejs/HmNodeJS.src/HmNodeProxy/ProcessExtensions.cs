/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;

// .NET の Process の MainWindowClose や MainWindowHandle は、相手が Visible でないと、反応しないので、
// 以下のようなネイティブ経由でのラップ関数がどうしても必要。
// 特に Formで  this.ShowInTaskbar = false; をしていたとしても、補足できるようにするためには、下記のようなものが必須となる。


public static class ProcessExtensions
{
    private static class Win32
    {
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr SendMessage(IntPtr hWnd, UInt32 Msg, IntPtr wParam, IntPtr lParam);

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool PostMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll", SetLastError = true)]
        public static extern bool PostThreadMessage(uint threadId, uint msg, IntPtr wParam, IntPtr lParam);

        public delegate bool EnumThreadDelegate(IntPtr hWnd, IntPtr lParam);
        [DllImport("user32.dll")]
        public static extern bool EnumThreadWindows(uint dwThreadId, EnumThreadDelegate lpfn, IntPtr lParam);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
    }

    // プロセスへのSendMessage関数。拡張メソッド
    public static IntPtr SendMessage(this Process p, out IntPtr hwnd, UInt32 msg, IntPtr wParam, IntPtr lParam)
    {
        hwnd = p.WindowHandles().FirstOrDefault();
        if (hwnd != IntPtr.Zero)
        {
            return Win32.SendMessage(hwnd, msg, wParam, lParam);
        }
        else
        {
            return IntPtr.Zero;
        }
    }

    // プロセスへのPostMessage関数。拡張メソッド
    public static bool PostMessage(this Process p, out IntPtr hwnd, UInt32 msg, IntPtr wParam, IntPtr lParam)
    {
        hwnd = p.WindowHandles().FirstOrDefault();
        if (hwnd != IntPtr.Zero)
        {
            return Win32.PostMessage(hwnd, msg, wParam, lParam);
        }
        else
        {
            return false;
        }
    }

    // プロセスへのPostThreadMessage関数。拡張メソッド。
    public static bool PostThreadMessage(this Process p, UInt32 msg, IntPtr wParam, IntPtr lParam, bool ensureTargetThreadHasWindow = true)
    {
        uint targetThreadId = 0;
        if (ensureTargetThreadHasWindow)
        {
            IntPtr hwnd = p.WindowHandles().FirstOrDefault();
            uint processId = 0;
            if (hwnd != IntPtr.Zero)
                targetThreadId = Win32.GetWindowThreadProcessId(hwnd, out processId);
        }
        else
        {
            targetThreadId = (uint)p.Threads[0].Id;
        }
        if (targetThreadId != 0)
        {
            return Win32.PostThreadMessage(targetThreadId, msg, wParam, lParam);
        }
        else
        {
            return false;
        }
    }

    // プロセスに属するトップレベルのウィンドウハンドルの一覧を得る
    public static IEnumerable<IntPtr> WindowHandles(this Process process)
    {
        var handles = new List<IntPtr>();
        foreach (ProcessThread thread in process.Threads)
        {
            Win32.EnumThreadWindows((uint)thread.Id, (hWnd, lParam) => { handles.Add(hWnd); return true; }, IntPtr.Zero);
        }
        return handles;
    }

    public static bool WaitForShowWindow(this Process proc)
    {
        for (int i = 0; i < 60; i++)
        {
            System.Threading.Thread.Sleep(50);
            proc.Refresh();

            foreach (var w in proc.WindowHandles())
            {
                System.Threading.Thread.Sleep(50);
                return true;
            }
        }

        return false;
    }

    public static void SendMessageCloseWindow(this Process proc)
    {
        var handles = proc.WindowHandles();
        foreach (var w in handles)
        {
            IntPtr hwndMessageWasSentTo = IntPtr.Zero; //this will receive a non-zero value if SendMessage was called successfully
            uint msg = 0x0010; // 0x0010 = WM_CLOSE The message you want to send
            IntPtr wParam = IntPtr.Zero; //The wParam value to pass to SendMessage
            IntPtr lParam = IntPtr.Zero; //The lParam value to pass to SendMessage

            IntPtr h = w;
            proc.SendMessage(out h, msg, wParam, lParam);
        }


    }

}