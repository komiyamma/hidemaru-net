/*
 * Copyright (c) 2021 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */
 
using Hidemaru;
using System;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;
using Microsoft.Web.WebView2.WinForms;

internal partial class HmWebView2WebBrowserModeForm : Form
{
    public static HmWebView2WebBrowserModeForm form { get; set; }

    private WebView2 wb;
    private String fontname;

    public HmWebView2WebBrowserModeForm(String fontname)
    {
        this.fontname = fontname;
        SetFormAttr();
        SetWebBrowserAttr();
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
        this.Width = 1;
        this.Height = 1;
        this.BackColor = Color.White;
        this.FormClosing += form_FormClosing;
        this.StartPosition = FormStartPosition.Manual;


        SetFormNoBorderAttr();
    }

    private void form_FormClosing(object sender, FormClosingEventArgs e)
    {
        Stop();
    }

    private string GetStartTitle()
    {
        string start_path = Hm.Edit.FilePath;
        if (start_path == null)
        {
            start_path = "about:blank";
        }
        return start_path;
    }
    private void SetWebBrowserAttr()
    {
        wb = new WebView2
        {
            Source = new Uri(GetStartTitle())
        };
        // wb.ScriptErrorsSuppressed = true;
        wb.Dock = DockStyle.Fill;
        this.Controls.Add(wb);
    }

    public String GetWebBrowserDocumentText()
    {
        if (wb != null)
        {
            var docHtml = wb.ExecuteScriptAsync("document.documentElement.outerHTML;");
            docHtml.Wait();
            return docHtml.Result ?? "";
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
            bool s1 = TimerTick_Notify(strCurFileName);
            if (!s1)
            {
                return;
            }

            // 表示しているUrlアドレス内容そのものに終了のシグナルがあれば
            bool s2 = TimerTick_CloseFromUrl();
            if (!s2)
            {
                return;
            }

            bool s3 = IsFormHideCondition();
            if (s3)
            {
                HideForm();
                return;
            }

            ShowForm();

            UpdateBrowserContent(strCurFileName);
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex.GetType());
            System.Diagnostics.Trace.WriteLine(ex.Message);
            System.Diagnostics.Trace.WriteLine(ex.StackTrace);
        }
    }

    private bool IsFormHideCondition()
    {
        // マウスの下のウィンドウが現在のプロセスではない
        if (!IsUnderWindowIsCurrentProcessWindow())
        {
            return true;
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
                return true;
            }

            // アクティブウィンドウがよそにある
            if (!IsActiveWindowIsHidemaruMainWindow())
            {
                return true;
            }
        }

        // 自分が先頭タブウィンドウではない
        IntPtr hWnd = Hm.WindowHandle;
        var list = GetWindowHidemaruHandleList();
        if (list.Count > 0 && list[0] != hWnd)
        {
            return true;
        }

        return false;
    }


    private void UpdateBrowserContent(string strCurFileName)
    {
        // ファイル名が有効ならば、それをWebBrowserでナビゲート
        if (strCurFileName.Length > 0)
        {
            if (strPrevFileName != strCurFileName)
            {
                strPrevFileName = strCurFileName;
                Uri u = new Uri(strCurFileName);
                if (wb != null)
                {
                    wb.Source = u;
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
                    wb.NavigateToString(strCurTotalText);
                }
            }
        }
    }

    // 特定の文字列がURLに含まれていたら、HmWebBrowserModeを終了する。
    bool TimerTick_CloseFromUrl()
    {
        if (wb != null)
        {
            string url = wb.Source.ToString();
            if ( String.IsNullOrEmpty(url))
            {
                if (url.ToLower().Contains("close_hmwebbrowsermode"))
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
                // wb.Dispose();
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
