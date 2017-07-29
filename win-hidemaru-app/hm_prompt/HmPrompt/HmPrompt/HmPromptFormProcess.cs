using Hidemaru;
using System;
using System.Diagnostics;
using System.Text;

internal partial class HmPromptForm
{
    String promptCommand = "";
    String promptArguments = "";

    Boolean isSettingMode = false;

    public System.Diagnostics.Process p;
    IntPtr processWindowHandle;

    void InitProcessAttr()
    {

        //Processオブジェクトを作成
        p = new Process();
        p.StartInfo.FileName = this.promptCommand;
        p.StartInfo.Arguments = this.promptArguments;
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
        int iOutputPaneWidth = rectOutputPane.Right - rectOutputPane.Left;
        if (iWndHidemaruWidth - iOutputPaneWidth < 100)
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    IntPtr iWntRootHidemaru;
    RECT rectRootHidemaru;

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
        iWntRootHidemaru = hWndHidemaru;
        IntPtr hParent = GetParent(hWndHidemaru);
        if (hParent != IntPtr.Zero)
        {
            iWntRootHidemaru = hParent;
        }
        // ここだけはウィンドウ
        GetWindowRect(iWntRootHidemaru, out rectRootHidemaru);



        hWndOutputPane = FindWindowEx(hWndHidemaru, IntPtr.Zero, "HM32OutputPane", IntPtr.Zero);
        GetWindowRect(hWndOutputPane, out rectOutputPane);

        hWndOutputPaneServer = FindWindowEx(hWndOutputPane, IntPtr.Zero, "HM32OutputPaneServer", IntPtr.Zero);
        GetWindowRect(hWndOutputPaneServer, out rectOutputPaneServer);

        RECT rectWndHidemaru;
        GetWindowRect(hWndHidemaru, out rectWndHidemaru);

        iWndHidemaruWidth = rectWndHidemaru.Right - rectWndHidemaru.Left;
        iWndHidemaruHeight = rectWndHidemaru.Bottom - rectWndHidemaru.Top;
        int iOutputServerWidth = rectOutputPaneServer.Right - rectOutputPaneServer.Left;

        if (IsHmOutputPaneIsBottomOrTop())
        {
            SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, iWndHidemaruWidth * 1 / 2, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);
        }
        else
        {
            SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPaneServer.Right - rectOutputPaneServer.Left, iWndHidemaruHeight * 1 / 2, SWP_NOMOVE);
        }

        if (!bStyleChanged)
        {
            IsHmOutputPaneIsBottomOrTop();
            // アトリビュート設定可能モード以外なら 枠とかキャプションは消去
            if (!isSettingMode)
            {
                uint style = (uint)GetWindowLong(processWindowHandle, (int)WindowLongFlags.GWL_STYLE);
                if ((style & (uint)WindowStyles.WS_CAPTION) > 0)
                {
                    style = style ^ (uint)WindowStyles.WS_CAPTION;
                }
                if ((style & (uint)WindowStyles.WS_THICKFRAME) > 0)
                {
                    style = style ^ (uint)WindowStyles.WS_THICKFRAME;
                }

                SetWindowLong(processWindowHandle, (int)WindowLongFlags.GWL_STYLE, style);
            }
            IntPtr ret = SetParent(processWindowHandle, hWndOutputPane);

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
                return;
            }


            if (p.HasExited)
            {
                if (IsHmOutputPaneIsBottomOrTop())
                {
                    SetWindowPos(iWntRootHidemaru, IntPtr.Zero, 0, 0, rectRootHidemaru.Right - rectRootHidemaru.Left + 2, rectRootHidemaru.Bottom - rectRootHidemaru.Top + 2, SWP_NOMOVE);
                    SetWindowPos(iWntRootHidemaru, IntPtr.Zero, 0, 0, rectRootHidemaru.Right - rectRootHidemaru.Left, rectRootHidemaru.Bottom - rectRootHidemaru.Top, SWP_NOMOVE);
                }
                else
                {
                    SetWindowPos(iWntRootHidemaru, IntPtr.Zero, 0, 0, rectRootHidemaru.Right - rectRootHidemaru.Left + 2, rectRootHidemaru.Bottom - rectRootHidemaru.Top + 2, SWP_NOMOVE);
                    SetWindowPos(iWntRootHidemaru, IntPtr.Zero, 0, 0, rectRootHidemaru.Right - rectRootHidemaru.Left, rectRootHidemaru.Bottom - rectRootHidemaru.Top, SWP_NOMOVE);
                }
                this.Close();
            }
            else
            {
                tick_OutputResize();
            }

            if (bStyleChanged)
            {
                IntPtr isActive = GetActiveWindow();

                uint uFlags = 0;
                if (isActive == processWindowHandle)
                {
                    uFlags = 0;
                }
                else
                {
                    uFlags = SWP_NOACTIVATE;
                }

                if (IsHmOutputPaneIsBottomOrTop())
                {
                    SetWindowPos(processWindowHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, uFlags);
                }
                else
                {
                    SetWindowPos(processWindowHandle, IntPtr.Zero, 3, iWndHidemaruHeight * 1 / 2 + 3, rectOutputPaneServer.Right - rectOutputPaneServer.Left - 3, iWndHidemaruHeight * 1 / 2 - 5, uFlags);
                }
            }

        }
        catch (Exception)
        {

        }
    }



}

