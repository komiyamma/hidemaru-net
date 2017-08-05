using Hidemaru;
using System;
using System.Drawing;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;

internal partial class HmWebBrowserModeForm : Form
{
    public static HmWebBrowserModeForm form { get; set; }

    private WebBrowser wb = new WebBrowser();
    private String fontname;
//    private Color tcolor;
//    private Button bt;

    public HmWebBrowserModeForm(String fontname)
    {
        this.fontname = fontname;
//        this.tcolor = tcolor;
        SetFormAttr();
        SetWebBrowserAttr();
//        SetButtonAttr();
        SetTimerAttr();
    }

    private void SetTimerAttr()
    {
        timer = new Timer();
        timer.Tick += Timer_Tick;
        timer.Enabled = true;
        timer.Interval = 10;
        timer.Start();
    }

    private void SetFormAttr()
    {
        /*
        this.KeyPreview = true;
        this.KeyDown += HmWebBrowserModeForm_KeyDown;
        */
        this.Width = 1;
        this.Height = 1;
        this.BackColor = Color.White;
        this.FormClosing += form_FormClosing;
        this.StartPosition = FormStartPosition.Manual;


        SetFormNoBorderAttr();
    }

    /*
    private void HmWebBrowserModeForm_KeyDown(object sender, KeyEventArgs e)
    {
        //受け取ったキーを表示する
        System.Diagnostics.Trace.WriteLine(e.KeyCode);
    }
    */

    /*
    private void SetButtonAttr()
    {
        bt = new Button();
        bt.Width = 4;
        bt.Height = 4;
        bt.BackColor = tcolor;
        bt.Left = 0;
        bt.Top = 0;
        bt.FlatStyle = FlatStyle.Flat;
        this.Controls.Add(bt);
    }
    */

    private void form_FormClosing(object sender, FormClosingEventArgs e)
    {
        Stop();
    }

    private void SetWebBrowserAttr()
    {
        wb.ScriptErrorsSuppressed = true;
        wb.Dock = DockStyle.Fill;
        this.Controls.Add(wb);
    }

    public String GetWebBrowserDocumentText()
    {
        if (wb != null)
        {
            return wb.DocumentText ?? "";
        }
        return "";
    }

    public String GetWebBrowserDocumentTextAsEncoding(Encoding encoding)
    {
        if (wb != null)
        {
            if (wb.DocumentText != null)
            {
                MemoryStream reader = (MemoryStream)this.wb.DocumentStream;
                byte[] bytes = reader.ToArray();
                string html = encoding.GetString(bytes);
                return html;
            }
        }
        return "";
    }

    // 自分の位置をマウスの位置に従って移動
    private void MoveLocation()
    {
        IntPtr hWnd = Hm.WindowHandle;

        if (hWnd != null)
        {
            IntPtr hHm32Client = FindWindowEx(hWnd, IntPtr.Zero, "HM32CLIENT", IntPtr.Zero);

            if (hHm32Client != null)
            {
                RECT rect;
                GetWindowRect(hHm32Client, out rect);

                const int border = 2;

                this.SuspendLayout();
                this.Left = rect.Left + border;
                //Y座標を取得する
                this.Top = rect.Top + border;

                this.Width = rect.Right - rect.Left - border * 2;

                this.Height = rect.Bottom - rect.Top - border * 2;

                if (wb != null)
                {
                    wb.Left = 0;
                    wb.Top = 0;
                    wb.Width = this.Width;
                    wb.Height = this.Height;
                }

                /*
                if (bt != null)
                {
                    bt.Left = this.Width - 4;
                    bt.Top = 0;
                }
                */
                this.ResumeLayout();
            }
        }

    }

    private void HideForm()
    {
        this.Hide();
    }

    private void ShowForm()
    {
        this.Show();
    }

    Timer timer;
    int nTickCounter = 0;

    String strPrevFileName = "";
    String strPrevTotalText = "";
    private void Timer_Tick(object sender, EventArgs e)
    {
        // 移動についていくのは細かなタイミングで
        MoveLocation();

        try
        {
            // それに比べれば、内容を更新するのは、粒度を荒く
            nTickCounter++;
            if (nTickCounter % (50 / timer.Interval) != 0) // 手抜き
            {
                return;
            }

            // ファイル名の変化をとらえ、終了を判断する
            String strCurFileName = Hm.Edit.FilePath ?? "";
            bool s1 = Timer_Tick_Notify(strCurFileName);
            if (!s1)
            {
                return;
            }

            // 表示しているUrlアドレス内容そのものに終了のシグナルがあれば
            bool s2 = Timer_Tick_CloseFromUrl();
            if (!s2)
            {
                return;
            }

            // マウスの下のウィンドウが現在のプロセスではない
            if (!IsUnderWindowIsCurrentProcessWindow())
            {
                HideForm();
                return;
            }

            // アクティブフォームが自分なら通過
            if (Form.ActiveForm == this)
            {

            }
            else
            { 
                // 最前面ウィンドウが自分自身のメインウィンドウではない
                if (!IsForegroundWindowIsHidemaruMainWindow())
                {
                    HideForm();
                    return;
                }

                // アクティブウィンドウがよそにある
                if (!IsActiveWindowIsHidemaruMainWindow())
                {
                    HideForm();
                    return;
                }
            }

            // 自分が先頭タブウィンドウではない
            IntPtr hWnd = Hm.WindowHandle;
            var list = GetWindowHidemaruHandleList();
            if (list.Count > 0 && list[0] != hWnd)
            {
                HideForm();
                return;
            }


            ShowForm();

            // ファイル名が有効ならば、それをWebBrowserでナビゲート
            if (strCurFileName.Length > 0)
            {
                if (strPrevFileName != strCurFileName)
                {
                    strPrevFileName = strCurFileName;
                    Uri u = new Uri(strCurFileName);
                    if (wb != null)
                    {
                        wb.Navigate(u);
                    }
                }
            }

            // 名前が無いのならテキスト内容をそのまま
            else
            {
                var strCurTotalText = Hm.Edit.TotalText;
                if (strPrevTotalText != strCurTotalText)
                {
                    strPrevTotalText = strCurTotalText;
                    if (wb != null)
                    {
                        wb.DocumentText = strCurTotalText;
                    }
                }
            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.GetType());
            System.Diagnostics.Trace.WriteLine(ex.Message);
            System.Diagnostics.Trace.WriteLine(ex.StackTrace);
        }
    }

    // 特定の文字列がURLに含まれていたら、HmWebBrowserModeを終了する。
    bool Timer_Tick_CloseFromUrl()
    {
        if (wb != null)
        {
            if (wb.Url != null)
            {
                // System.Diagnostics.Trace.WriteLine(wb.Url.ToString().ToLower());
                if (wb.Url.ToString().ToLower().Contains("close_hmwebbrowsermode"))
                {
                    this.isClosed = true;
                    this.Close();
                    return false;
                }
            }
        }
        return true;
    }

    private void Stop()
    {
        try
        {
            if (timer != null)
            {
                timer.Stop();
                timer = null;
            }
            if (wb != null)
            {
                wb.Dispose();
                wb = null;
            }

            isClosed = true;
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

}
