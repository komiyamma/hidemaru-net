using Hidemaru;
using System;
using System.Drawing;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using CefSharp;

namespace CefSharp
{
    internal class StringVisiterImplements : CefSharp.IStringVisitor
    {
        public string DocumentText { get; set; }
        public void Dispose()
        {
            DocumentText = "";
        }

        public void Visit(string str)
        {
            // System.Diagnostics.Trace.WriteLine(str);
            DocumentText = str;
        }
    }
}

internal partial class HmChromeBrowserModeForm : Form
{
    public static HmChromeBrowserModeForm form { get; set; }

    private CefSharp.WinForms.ChromiumWebBrowser wb;
    private string wbUrl = "";
    private StringVisiterImplements sve = new StringVisiterImplements();
    private String fontname;
    //    private Color tcolor;
    //    private Button bt;

    public HmChromeBrowserModeForm(String fontname)
    {
        NextCommandMustClose = false;
        try
        {
            this.fontname = fontname;
            //        this.tcolor = tcolor;
            SetFormAttr();
            SetWebBrowserAttr();
            //        SetButtonAttr();
            SetTimerAttr();
            CreateSharedMemory();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
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

    public bool NextCommandMustClose { get; set; }
    private void form_FormClosing(object sender, FormClosingEventArgs e)
    {
        if (NextCommandMustClose)
        {
            Stop();
        }
        else
        {
            e.Cancel = true;
            HideStop();
        }
    }

    private void SetWebBrowserAttr()
    {
        try
        {
            this.strPrevFileName = "";
            this.strPrevTotalText = "";
            // ファイル名が有効ならば、それをWebBrowserでナビゲート
            var strStartFileName = Hm.Edit.FilePath ?? "";
            if (wb == null)
            {
                if (!Cef.IsInitialized) { 
                    var settings = new CefSharp.CefSettings();

                    // By default CEF uses an in memory cache, to save cached data e.g. passwords you need to specify a cache path
                    // NOTE: The executing user must have sufficient privileges to write to this folder.
                    //settings.CachePath = @"C:\Users\0300002167\AppData\Local\Google\Chrome\User Data";
                    settings.LocalesDirPath = Hm.Macro.Var["currentmacrodirectory"] + @"\Locales";
                    settings.AcceptLanguageList = "ja-JP";
                    settings.Locale = "ja";
                    CefSharp.Cef.Initialize(settings);
                }
                wb = new CefSharp.WinForms.ChromiumWebBrowser(strStartFileName);

                CefSharp.BrowserSettings bs = new CefSharp.BrowserSettings
                {
                    LocalStorage = CefSharp.CefState.Enabled,
                    FileAccessFromFileUrls = CefSharp.CefState.Enabled,
                    UniversalAccessFromFileUrls = CefSharp.CefState.Enabled,
                    ApplicationCache = CefSharp.CefState.Enabled,
                    AcceptLanguageList = "ja-JP",
                };

                wb.BrowserSettings = bs;

                wb.Dock = DockStyle.Fill;
                wb.LoadingStateChanged += wb_LoadingStateChanged;
                wb.AddressChanged += wb_AddressChanged;

                if (strStartFileName == "")
                {
                    wb_LoadHtml(Hm.Edit.TotalText);
                }

                this.Controls.Add(wb);
            }
            else
            {
                if (strStartFileName == "")
                {
                    wb.ResetText();
                    wb_LoadHtml(Hm.Edit.TotalText);
                }
                else
                {
                    wb.ResetText();
                    wb.Load(strStartFileName);
                    wb.Refresh();
                }

            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

    private void wb_AddressChanged(object sender, AddressChangedEventArgs e)
    {
        try
        {
            wbUrl = e.Address ?? "";
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.Message);
        }
    }

    private void wb_LoadingStateChanged(object sender, LoadingStateChangedEventArgs e)
    {
        try
        {
            wb.GetBrowser().MainFrame.GetSource(sve);
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.Message);
        }
    }

    public void wb_LoadHtml(string html)
    {
        try
        {
            // なぜかこんな感じでまともなサイト指定しておかないと、第２引数を省略すると文字化けする
            wb.LoadHtml(html, "http://www.google.com");
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.Message);
        }

    }

    public String GetWebBrowserDocumentText()
    {
        if (wb != null)
        {
            return sve.DocumentText;
        }
        return "";
    }

    public String GetWebBrowserDocumentTextAsEncoding(Encoding encoding)
    {
        return GetWebBrowserDocumentText();
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

                /*
                if (wb != null)
                {
                    wb.Left = 0;
                    wb.Top = 0;
                    wb.Width = this.Width;
                    wb.Height = this.Height;
                }
                */

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
                    wb.Load(strCurFileName);
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
                        wb_LoadHtml(strCurTotalText);
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
            if (wbUrl != null)
            {
                // System.Diagnostics.Trace.WriteLine(wb.Url.ToString().ToLower());
                if (wbUrl.ToString().ToLower().Contains("close_hmwebbrowsermode"))
                {
                    this.isHideStop = true;
                    HideStop();
                    return false;
                }
            }
        }
        return true;
    }

    private void HideStop()
    {
        try
        {
            this.Hide();
            if (timer != null)
            {
                timer.Stop();
                timer = null;
            }

            isHideStop = true;
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

    public void Stop()
    {
        try
        {
            HideStop();

            sve.Dispose();
            sve = null;

            Cef.Shutdown();

            if (wb != null)
            {
                wb.Dispose();
                wb = null;
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }


}
