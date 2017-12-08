/*
 * This codes is licensed under CC0 1.0 Universal
 */

using System;
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace HmAllKill
{
    class Program
    {
        /*
        [DllImport("user32.dll")]
        static extern IntPtr FindWindow(string className, string windowName);
        [DllImport("user32.dll")]
        static extern IntPtr FindWindowEx(IntPtr parent, IntPtr child, string className, string windowName);
        [DllImport("user32.dll")]
        static extern bool GetWindowRect(HandleRef handle, out RECT rct);

        [StructLayout(LayoutKind.Sequential)]
        struct RECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        static void RefreshTray()
        {
            IntPtr taskbar_Handle = FindWindow("Shell_Traywnd", "");
            IntPtr tray_Handle = FindWindowEx(taskbar_Handle, IntPtr.Zero, "TrayNotifyWnd", "");

            RECT rct;

            if (!(GetWindowRect(new HandleRef(null, tray_Handle), out rct)))
            {
            }

            System.Drawing.Point init = Control.MousePosition;

            for (int i = rct.Left; i < rct.Right - 20; i++)
            {
                Cursor.Position = new System.Drawing.Point(i, (rct.Bottom + rct.Top) / 2);
            }

            Cursor.Position = init;
        }
        */
        static void Main(string[] args)
        {
            //notepadのプロセスを取得
            System.Diagnostics.Process[] ps1 = System.Diagnostics.Process.GetProcessesByName("hidemaru");

            foreach (System.Diagnostics.Process p in ps1)
            {
                //プロセスを強制的に終了させる
                p.Kill();
            }

            // RefreshTray();
        }
    }
}
