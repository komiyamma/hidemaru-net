using Hidemaru;
using System;
using System.Diagnostics;
using System.Text;

internal partial class HmPromptForm
{
    ProcessStartInfo constructPsi = new ProcessStartInfo();

    Boolean processSettingMode = false;

    public Process process;
    IntPtr processWindowHandle;

    void InitProcessAttr()
    {

        //Processオブジェクトを作成
        process = new Process();
        process.StartInfo = constructPsi.Copy();
        //起動
        process.Start();

        // 対象プロセスのメインウィンドウが発生するまで待機。
        // まぁすぐ発生するでしょ…
        while (true)
        {
            System.Threading.Thread.Sleep(10);
            processWindowHandle = process.MainWindowHandle;
            StringBuilder sb = new StringBuilder(256);
            GetClassName(processWindowHandle, sb, sb.Capacity);

            if (sb.ToString() == "ConsoleWindowClass")
            {
                // isAttachConsole = AttachConsole((uint)p.Id);
                break;
            }
        }

    }

    IntPtr iWntRootHidemaru;

    bool isPromptWindowStyleChanged = false;
    private void UpdateOutputPaneServerAndPromptLocation()
    {
        // hidemaruhandle(0)と呼ばれる値
        IntPtr hWndHidemaru = Hm.WindowHandle;
        // 
        RECT rectWndHidemaru;
        GetWindowRect(hWndHidemaru, out rectWndHidemaru);

        // ルートの秀丸ウィンドウを求める 
        iWntRootHidemaru = IntPtr.Zero;
        // hidemaruhandle(0)にさらに親はあるか？(タブモードだと親があり、非タブモードだとなし)
        IntPtr hParent = GetParent(hWndHidemaru);
        
        // 親があれば、そいつが秀丸
        if (hParent != IntPtr.Zero)
        {
            iWntRootHidemaru = hParent;
        // 親がなければ、hidemaruhandle(0)相当が親
        } else
        {
            iWntRootHidemaru = hWndHidemaru;
        }

        // アウトプット枠を探す。
        IntPtr hWndOutputPane = FindWindowEx(hWndHidemaru, IntPtr.Zero, "HM32OutputPane", IntPtr.Zero);
        RECT rectOutputPane;
        GetWindowRect(hWndOutputPane, out rectOutputPane);

        // アウトプット枠サーバーを探す。ちなみに、マクロにあるアウトプット枠用に用意されているGetWindowHandleはこの値
        IntPtr hWndOutputPaneServer = FindWindowEx(hWndOutputPane, IntPtr.Zero, "HM32OutputPaneServer", IntPtr.Zero);
        RECT rectOutputPaneServer;
        GetWindowRect(hWndOutputPaneServer, out rectOutputPaneServer);

        // プロンプトが秀丸内にくっついた後
        if (isPromptWindowStyleChanged)
        {
            // スタイルを変える度に、プロンプトがアクティブになってしまうので、
            // そういったことを防止する。
            // すてにアクティブなら再度アクティブにしつつサイズを変更し、
            // そうでなければ、アクティブにせずサイズを変更する
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

            int iWndHidemaruWidth = rectWndHidemaru.Right - rectWndHidemaru.Left;
            int iWndHidemaruHeight = rectWndHidemaru.Bottom - rectWndHidemaru.Top;
            int iOutputServerWidth = rectOutputPaneServer.Right - rectOutputPaneServer.Left;

            // アウトプット枠が、下部もしくは上部にあるのか？ LeftやRightにあったらfalse
            var IsHmOutputPaneIsBottomOrTop = new Func<bool>(() =>
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
            );

            // 秀丸内で、アウトプット枠は、TopやBottomにくっついているなら
            if (IsHmOutputPaneIsBottomOrTop())
            {
                // アウトプット枠を左半分ぐらいにして
                SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, iWndHidemaruWidth * 1 / 2, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);
                // プロンプトはその右に配置
                SetWindowPos(processWindowHandle, IntPtr.Zero, iWndHidemaruWidth * 1 / 2 + 3, 3, iWndHidemaruWidth * 1 / 2 - 5, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, uFlags);
            }
            // 秀丸内で、アウトプット枠は、LeftやRightにくっついている
            else
            {
                // アウトプット枠は、上半分ぐらいにして
                SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPaneServer.Right - rectOutputPaneServer.Left, iWndHidemaruHeight * 1 / 2, SWP_NOMOVE);
                // プロンプトはその下に配置
                SetWindowPos(processWindowHandle, IntPtr.Zero, 3, iWndHidemaruHeight * 1 / 2 + 3, rectOutputPaneServer.Right - rectOutputPaneServer.Left - 3, iWndHidemaruHeight * 1 / 2 - 5, uFlags);
            }
        }

        // まだスタイルを変更していない
        if (!isPromptWindowStyleChanged)
        {
            // アトリビュート設定可能モード以外なら 枠とかキャプションは消去
            if (!processSettingMode)
            {
                // 現在のスタイル
                uint style = (uint)GetWindowLong(processWindowHandle, (int)WindowLongFlags.GWL_STYLE);
                // キャプションなし
                if ((style & (uint)WindowStyles.WS_CAPTION) > 0)
                {
                    style = style ^ (uint)WindowStyles.WS_CAPTION;
                }
                // フレームなし
                if ((style & (uint)WindowStyles.WS_THICKFRAME) > 0)
                {
                    style = style ^ (uint)WindowStyles.WS_THICKFRAME;
                }
                // スタイル上書き
                SetWindowLong(processWindowHandle, (int)WindowLongFlags.GWL_STYLE, style);
            }
            // 親をトップウィンドウ→秀丸のアウトプットパネルにする
            IntPtr ret = SetParent(processWindowHandle, hWndOutputPane);

            // スタイルは変更した
            isPromptWindowStyleChanged = true;
        }
    }

    // アウトプット枠からプロンプト消えてるはずなので、元へと戻す
    private void RevertOutputPaneServerLocation()
    {
        RECT rectRootHidemaru;
        // ここだけはウィンドウ
        GetWindowRect(iWntRootHidemaru, out rectRootHidemaru);

        // 秀丸本体にリサイズの通知
        SetWindowPos(iWntRootHidemaru, IntPtr.Zero, 0, 0, rectRootHidemaru.Right - rectRootHidemaru.Left + 2, rectRootHidemaru.Bottom - rectRootHidemaru.Top + 2, SWP_NOMOVE);
        SetWindowPos(iWntRootHidemaru, IntPtr.Zero, 0, 0, rectRootHidemaru.Right - rectRootHidemaru.Left, rectRootHidemaru.Bottom - rectRootHidemaru.Top, SWP_NOMOVE);
    }

    private void Update_TickProcessWindow(object sender, EventArgs e)
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
            if (process == null)
            {
                return;
            }

            if (process.HasExited)
            {
                RevertOutputPaneServerLocation();
                this.Close();
                return;
            }
            else
            {
                UpdateOutputPaneServerAndPromptLocation();
            }
        }
        catch (Exception)
        {

        }
    }
}

