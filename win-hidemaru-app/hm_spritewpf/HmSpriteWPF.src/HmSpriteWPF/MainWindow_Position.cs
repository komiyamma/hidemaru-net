/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Threading;

namespace HmSpriteWPF
{
    partial class MainWindow : Window
    {
        // 対象とするウィンドウ位置を監視するためのタイマー。自分自身の位置を更新するため
        DispatcherTimer timer_Position;

        private void SetPositionTimerAttribute()
        {
            this.timer_Position = new DispatcherTimer();
            timer_Position.Interval = new TimeSpan(0, 0, 0, 0, 33);
            timer_Position.Tick += Timer_Position_Tick;
            timerList.Add(timer_Position);
            timer_Position.Start();
        }

        int prevLeft;
        int prevTop;
        private void ResetFollowPosition()
        {
            prevLeft = 0;
            prevTop = 0;
        }

        private void UpdateFollowPosition()
        {
            // 対象とするウィンドウが見つからん
            if (options.wndHandle == IntPtr.Zero)
            {
                // やめ
                StopAndClose();
                return;
            }
            IntPtr hWndTarget = options.wndHandle;

            // 対象とするウィンドウのデスクトップ上での座標の確保
            RECT rect = new RECT();
            GetWindowRect(hWndTarget, ref rect);

            // 位置を更新しまくるのを間引くため。
            if (prevLeft != rect.left || prevTop != rect.top)
            {
                prevLeft = rect.left;
                prevTop = rect.top;

                // System.Console.WriteLine(rect.left);
                // System.Console.WriteLine(rect.top);
                // 対象とするウィンドウのデスクトップ上での幅と高さ
                int w = rect.right - rect.left;
                int h = rect.bottom - rect.top;

                // 自分自身のフォームの幅と高さは画像の幅と高さ
                this.Width = image.Source.Width;// rect.right - rect.left;
                this.Height = image.Source.Height; // rect.bottom - rect.top;

                // 位置は、対象とするウィンドウの中央へ
                this.Left = rect.left + (w - this.Width) / 2; // rect.left;
                this.Top = rect.top + (h - this.Height) / 2; // rect.top;

                if (image != null) { 
                    image.Visibility = Visibility.Visible;
                }
            }
        }

        private void Timer_Position_Tick(Object sender, EventArgs e)
        {
            UpdateFollowPosition();
        }

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr FindWindow(String lpClassName, IntPtr lpWindowName);

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("user32.dll")]
        internal static extern bool GetWindowRect(IntPtr hwnd, ref RECT lpRect);

        [StructLayout(LayoutKind.Sequential)]
        internal struct RECT
        {
            internal int left;
            internal int top;
            internal int right;
            internal int bottom;
        }
    }
}
