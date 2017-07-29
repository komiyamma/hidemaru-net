using Hidemaru;
using System;
using System.Diagnostics;
using System.Text;

internal partial class HmPromptForm
{
    public enum Command { cmd = 1, powershell = 2 };
    Command command = Command.powershell;
    public System.Diagnostics.Process p;
    IntPtr guestHandle;
    // bool isAttachConsole = false;

    public IntPtr GetCommandType()
    {
        return (IntPtr)(int)command;
    }

    void InitProcessAttr()
    {

        //Processオブジェクトを作成
        p = new Process();
        if (command == Command.cmd)
        {
            //ComSpec(cmd.exe)のパスを取得して、FileNameプロパティに指定
            p.StartInfo.FileName = System.Environment.GetEnvironmentVariable("ComSpec");
            //コマンドラインを指定（"/k"は実行後閉じないため）
            p.StartInfo.Arguments = @"/K";
        }
        else if (command == Command.powershell)
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
            guestHandle = p.MainWindowHandle;
            StringBuilder sb = new StringBuilder(256);
            GetClassName(guestHandle, sb, sb.Capacity);

            if (sb.ToString() == "ConsoleWindowClass")
            {
                ShowWindowAsync(guestHandle, (int)ShowWindowCommands.Hide);
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
        if (rectOutputPaneServer.Right- rectOutputPaneServer.Left > rectOutputPaneServer.Bottom - rectOutputPaneServer.Top)
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

    bool bStyleChanged = false;
    private void tick_OutputResize()
    {
        IntPtr hWndHidemaru = Hm.WindowHandle;

        hWndOutputPane = FindWindowEx(hWndHidemaru, IntPtr.Zero, "HM32OutputPane", IntPtr.Zero);
        GetClientRect(hWndHidemaru, out rectOutputPane);

        hWndOutputPaneServer = FindWindowEx(hWndOutputPane, IntPtr.Zero, "HM32OutputPaneServer", IntPtr.Zero);
        GetClientRect(hWndOutputPaneServer, out rectOutputPaneServer);

        RECT rectWndHidemaru;
        GetClientRect(hWndHidemaru, out rectWndHidemaru);

        iWndHidemaruWidth = rectWndHidemaru.Right - rectWndHidemaru.Left;
        iWndHidemaruHeight = rectWndHidemaru.Bottom - rectWndHidemaru.Top;
        int iOutputServerWidth = rectOutputPaneServer.Right - rectOutputPaneServer.Left;

        //if (IsHmOutputPaneIsBottomOrTop() ) { 
            SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, iWndHidemaruWidth * 1 / 2, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);
        //} else
        //{
        // SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPaneServer.Right - rectOutputPaneServer.Left, iWndHidemaruHeight * 1 / 2, SWP_NOMOVE);
        //}

        if (!bStyleChanged)
        {
            uint style = (uint)GetWindowLong(guestHandle, (int)WindowLongFlags.GWL_STYLE);
            uint remove = (uint)WindowStyles.WS_CAPTION | (uint)WindowStyles.WS_THICKFRAME;
            SetParent(guestHandle, hWndOutputPane);
            SetWindowLong(guestHandle, (int)WindowLongFlags.GWL_STYLE, style ^ remove);

            //if (IsHmOutputPaneIsBottomOrTop())
            // {
            SetWindowPos(guestHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
            //  }
            //  else
            //  {
            // SetWindowPos(guestHandle, IntPtr.Zero, 3, iWndHidemaruHeight * 1 / 2 + 3, rectOutputPaneServer.Right - rectOutputPaneServer.Left, iWndHidemaruHeight * 1 / 2 - 5, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
            // }

            ShowWindowAsync(guestHandle, (int)ShowWindowCommands.Show);

            bStyleChanged = true;
        }
    }

    // int i = 0;
    private void timer_TickProcessWindow(object sender, EventArgs e)
    {
        /*
        if (i % 100 == 0)
        {
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
        }
        i++;
        */

        try
        {
            if (p == null)
            {
                return;
            }


            if (p.HasExited)
            {
                SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPane.Right - rectOutputPane.Left, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);
                this.Close();
            }
            else
            {
                tick_OutputResize();
            }

            if (bStyleChanged) {
                IntPtr isActive = GetActiveWindow();
                if (isActive == guestHandle)
                {
                    // SetWindowPos(guestHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth - iWndHidemaruWidth * 1 / 2 - 5, rectOutputServer.Bottom - rectOutputServer.Top, 0);
                    //    if (IsHmOutputPaneIsBottomOrTop())
                    //     {
                    SetWindowPos(guestHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, 0);
                    //     }
                    //     else
                    //     {
                    // SetWindowPos(guestHandle, IntPtr.Zero, 3, iWndHidemaruHeight * 1 / 2 + 3, rectOutputPaneServer.Right - rectOutputPaneServer.Left, iWndHidemaruHeight * 1 / 2 - 5, 0);
                    //   }

                }
                else {
                    // SetWindowPos(guestHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth - iWndHidemaruWidth * 1 / 2 - 5, rectOutputServer.Bottom - rectOutputServer.Top, SWP_NOACTIVATE);
                    //      if (IsHmOutputPaneIsBottomOrTop())
                    //      {
                    //System.Diagnostics.Trace.WriteLine("横");
                        SetWindowPos(guestHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOACTIVATE);
                    //    }
                    //    else
                    //     {
                    //System.Diagnostics.Trace.WriteLine("縦");
                    // SetWindowPos(guestHandle, IntPtr.Zero, 3, iWndHidemaruHeight * 1 / 2 + 3, rectOutputPaneServer.Right - rectOutputPaneServer.Left, iWndHidemaruHeight * 1 / 2 - 5, SWP_NOACTIVATE);
                    //    }


                }
            }

        }
        catch (Exception)
        {

        }
    }



}

