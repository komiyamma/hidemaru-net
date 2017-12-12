/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

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

    IntPtr hWndRootHidemaru;

    RECT rectWndHidemaru;

    RECT rectOutputPane;

    IntPtr hWndOutputPaneServer;
    RECT rectOutputPaneServer;


    // アウトプット枠が、下部もしくは上部にあるのか？ LeftやRightにあったらfalse
    bool IsHmOutputPaneIsBottomOrTop()
    {
        int iOutputPaneWidth = rectOutputPane.Right - rectOutputPane.Left;
        int iWndHidemaruWidth = rectWndHidemaru.Right - rectWndHidemaru.Left;
        if (iWndHidemaruWidth - iOutputPaneWidth < 100)
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    bool isPromptWindowStyleChanged = false;
    private void UpdateOutputPaneServerAndPromptLocation()
    {
        // hidemaruhandle(0)と呼ばれる値
        IntPtr hWndHidemaru = Hm.WindowHandle;
        // 
        GetWindowRect(hWndHidemaru, out rectWndHidemaru);

        // ルートの秀丸ウィンドウを求める 
        hWndRootHidemaru = IntPtr.Zero;
        // hidemaruhandle(0)にさらに親はあるか？(タブモードだと親があり、非タブモードだとなし)
        IntPtr hParent = GetParent(hWndHidemaru);
        
        // 親があれば、そいつが秀丸
        if (hParent != IntPtr.Zero)
        {
            hWndRootHidemaru = hParent;
        // 親がなければ、hidemaruhandle(0)相当が親
        } else
        {
            hWndRootHidemaru = hWndHidemaru;
        }

        // アウトプット枠を探す。
        IntPtr hWndOutputPane = FindWindowEx(hWndHidemaru, IntPtr.Zero, "HM32OutputPane", IntPtr.Zero);
        GetWindowRect(hWndOutputPane, out rectOutputPane);

        // アウトプット枠サーバーを探す。ちなみに、マクロにあるアウトプット枠用に用意されているGetWindowHandleはこの値
        hWndOutputPaneServer = FindWindowEx(hWndOutputPane, IntPtr.Zero, "HM32OutputPaneServer", IntPtr.Zero);
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
            int iOutputPaneWidth = rectOutputPane.Right - rectOutputPane.Left;
            int iOutputPaneHeight = rectOutputPane.Bottom - rectOutputPane.Top;


            // 秀丸内で、アウトプット枠は、TopやBottomにくっついているなら
            if (IsHmOutputPaneIsBottomOrTop())
            {
                int original_height = rectOutputPaneServer.Bottom - rectOutputPaneServer.Top;
                // アウトプット枠を左半分ぐらいにして
                SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, iOutputPaneWidth / 2, original_height, SWP_NOMOVE);
                // プロンプトはその右に配置
                SetWindowPos(processWindowHandle, IntPtr.Zero, iOutputPaneWidth / 2 + 3, 3, iOutputPaneWidth / 2 - 5, original_height - 3, uFlags);
            }
            // 秀丸内で、アウトプット枠は、LeftやRightにくっついている
            else
            {
                int original_width = rectOutputPaneServer.Right - rectOutputPaneServer.Left;
                // アウトプット枠は、上半分ぐらいにして
                SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, original_width, iOutputPaneHeight / 2, SWP_NOMOVE);
                // プロンプトはその下に配置
                SetWindowPos(processWindowHandle, IntPtr.Zero, 3, iOutputPaneHeight / 2 + 5, original_width - 5, iOutputPaneHeight / 2 - 6, uFlags);
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
        GetWindowRect(hWndRootHidemaru, out rectRootHidemaru);

        // 秀丸内で、アウトプット枠は、TopやBottomにくっついているなら
        if (IsHmOutputPaneIsBottomOrTop())
        {
            // アウトプット枠(HmOutputPaneServerのサイズ)を元へと戻す
            SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPane.Right - rectOutputPane.Left, rectOutputPaneServer.Bottom - rectOutputPaneServer.Top, SWP_NOMOVE);
        }
        // 秀丸内で、アウトプット枠は、LeftやRightにくっついている
        else
        {
            // アウトプット枠(HmOutputPaneServerのサイズ)を元へと戻す
            SetWindowPos(hWndOutputPaneServer, IntPtr.Zero, 0, 0, rectOutputPaneServer.Right - rectOutputPaneServer.Left, rectOutputPane.Bottom - rectOutputPane.Top, SWP_NOMOVE);
        }

    }

    private void Update_TickProcessWindow(object sender, EventArgs e)
    {

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
    }
}

