/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;
using System.Text;
using System.Windows.Forms;

internal partial class HmPromptForm
{
    public enum ConsoleType { cmd = 1, powershell = 2 };
    ConsoleType consoleType = ConsoleType.powershell;
    public System.Diagnostics.Process p;
    IntPtr processWindowHandle;
    // bool isAttachConsole = false;

    public IntPtr GetCommandType()
    {
        return (IntPtr)(int)consoleType;
    }

    void InitProcessAttr()
    {
        bStyleChanged = false;

        //Processオブジェクトを作成
        p = new Process();
        if (consoleType == ConsoleType.cmd)
        {
            //ComSpec(cmd.exe)のパスを取得して、FileNameプロパティに指定
            p.StartInfo.FileName = System.Environment.GetEnvironmentVariable("ComSpec");
            //コマンドラインを指定（"/k"は実行後閉じないため）
            p.StartInfo.Arguments = @"/K";
        }
        else if (consoleType == ConsoleType.powershell)
        {
            p.StartInfo.FileName = "powershell";
            //コマンドラインを指定（"/NoExit"は実行後閉じないため）
            p.StartInfo.Arguments = "-NoExit -NoLogo";
        }

        //起動
        p.Start();
        while (true)
        {
            System.Threading.Thread.Sleep(5);
            processWindowHandle = p.MainWindowHandle;
            StringBuilder sb = new StringBuilder(256);
            GetClassName(processWindowHandle, sb, sb.Capacity);

            if (sb.ToString() == "ConsoleWindowClass")
            {
                // isAttachConsole = AttachConsole((uint)p.Id);
                break;
            }
        }

    }

    // アウトプット枠が、下部もしくは上部にあるのか？
    private bool IsHmOutputPaneIsBottomOrTop()
    {
        return true;
        System.Diagnostics.Trace.WriteLine(rectOutputPaneServer.Bottom - rectOutputPaneServer.Top);

        // 幅 > 高
        if (rectOutputPaneServer.Right - rectOutputPaneServer.Left > rectOutputPaneServer.Bottom - rectOutputPaneServer.Top)
        {
            return true;

        }
        return false;
    }


    IntPtr hWndOutputPaneServer;
    RECT rectOutputPaneServer;
    IntPtr hWndOutputPane;
    RECT rectOutputPane;

    int iWndHidemaruWidth;
    int iWndHidemaruHeight;

    static bool bStyleChanged = false;
    public static void ResetStyleChange()
    {
        bStyleChanged = false;
    }
    private void tick_OutputResize()
    {
        IntPtr hMemoryHidemaru = ReadSharedMemory();
        if (hMemoryHidemaru != IntPtr.Zero)
        {
            if (hWndHidemaru != hMemoryHidemaru)
            {
                ResetStyleChange();
            }
        }

        // hWndOutputPaneServerがいわゆる「アウトプット枠のウィンドウハンドル」
        // このサーバーは、秀丸のプロセスが異なっても、タブグループとして一緒になっている限りにおいては
        // 一緒である。

        // その親が、HmOutputPaneのウィンドウ
        hWndOutputPane = GetParent(hWndOutputPaneServer);
        if (hWndOutputPane == IntPtr.Zero)
        {
            System.Diagnostics.Trace.WriteLine("hWndOutputPane Zero Error");
        }
        // ウィンドウのサイズはいつでも変化しうる
        GetClientRect(hWndHidemaru, out rectOutputPane);

        // いわゆる「秀丸ハンドル」
        hWndHidemaru = GetParent(hWndOutputPane);

        if (hWndHidemaru == IntPtr.Zero)
        {
            System.Diagnostics.Trace.WriteLine("hWndHidemaru Zero Error");
        }

        // サイズはいつでも変化しうる
        GetClientRect(hWndOutputPaneServer, out rectOutputPaneServer);

        RECT rectWndHidemaru;
        GetClientRect(hWndHidemaru, out rectWndHidemaru);

        iWndHidemaruWidth = rectWndHidemaru.Right - rectWndHidemaru.Left;
        iWndHidemaruHeight = rectWndHidemaru.Bottom - rectWndHidemaru.Top;
        int iOutputServerWidth = rectOutputPaneServer.Right - rectOutputPaneServer.Left;

        // アウトプット枠自体のリサイズ
        SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, iWndHidemaruWidth * 1 / 2, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);

        if (!bStyleChanged)
        {
            System.Diagnostics.Trace.WriteLine("スタイル変化");

            uint style = (uint)GetWindowLong(processWindowHandle, (int)WindowLongFlags.GWL_STYLE);
            if ((style & (uint)WindowStyles.WS_CAPTION) > 0)
            {
                style = style ^ (uint)WindowStyles.WS_CAPTION;
            }
            if ((style & (uint)WindowStyles.WS_THICKFRAME) > 0)
            {
                style = style ^ (uint)WindowStyles.WS_THICKFRAME;
            }

            IntPtr ret = SetParent(processWindowHandle, hWndOutputPane);
            SetWindowLong(processWindowHandle, (int)WindowLongFlags.GWL_STYLE, style);
            bStyleChanged = true;
        }
    }

    private void timer_TickProcessWindow(object sender, EventArgs e)
    {
        /*
        if (isAttachConsole)
        {
            const int STD_OUTPUT_HANDLE = -11;
            const int FOREGROUND_GREEN = 2;
            const int FOREGROUND_INTENSITY = 8;
            const int BACKGROUND_BLUE = 16;

            CONSOLE_SCREEN_BUFFER_INFO screenBuffer = new CONSOLE_SCREEN_BUFFER_INFO();
            IntPtr hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            GetConsoleScreenBufferInfo(hConsole, out screenBuffer);

            SetConsoleTextAttribute(
                hConsole,
                FOREGROUND_GREEN
                | FOREGROUND_INTENSITY
                | BACKGROUND_BLUE);

            COORD dwReadCoord;
            dwReadCoord.X = 0;
            dwReadCoord.Y = 0;
            for (i = 0; i < screenBuffer.dwSize.Y; i++)
            {
                StringBuilder buffer = new StringBuilder(screenBuffer.dwSize.X + 1);
                uint lpNumberOfCharsRead = 0;
                ReadConsoleOutputCharacter(hConsole, buffer, (uint)screenBuffer.dwSize.X, dwReadCoord, out lpNumberOfCharsRead);
                System.Diagnostics.Trace.WriteLine(buffer);
            }
        } else
        {
            System.Diagnostics.Trace.WriteLine("失敗");
        }
        */

        try
        {
            if (p == null)
            {
                System.Diagnostics.Trace.WriteLine("プロセス無し");

                return;
            }


            if (p.HasExited)
            {
                System.Diagnostics.Trace.WriteLine("プロセスは終わった");
                SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPane.Right - rectOutputPane.Left, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);
                this.Stop();
            }
            else
            {
                tick_OutputResize();
            }

            if (bStyleChanged)
            {
                IntPtr isActive = GetActiveWindow();
                if (isActive == processWindowHandle)
                {
                    SetWindowPos(processWindowHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, 0);
                }
                else
                {
                    SetWindowPos(processWindowHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOACTIVATE);
                }
            }

        }
        catch (Exception)
        {

        }
    }



}

