using Hidemaru;
using System;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Windows.Forms;

internal partial class HmWebBrowserModeForm : Form
{
    public static HmWebBrowserModeForm form { get; set; }

    private WebBrowser wb = new WebBrowser();
    private String strFontName;

    public HmWebBrowserModeForm(String fontname)
    {
        this.strFontName = fontname;
        SetFormAttr();
        SetWebBrowserAttr();
        SetTimerAttr();
        CreateSharedMemory();
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

    private void SetWebBrowserAttr()
    {
        wb.ScriptErrorsSuppressed = true;
        this.Controls.Add(wb);
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
    public String GetPrevFileName()
    {
        return strPrevFileName;
    }
    String strPrevTotalText = "";
    private void Timer_Tick(object sender, EventArgs e)
    {
        MoveLocation();

        try
        {
            nTickCounter++;
            if (nTickCounter % (50 / timer.Interval) != 0) // 手抜き
            {
                return;
            }


            if (!IsUnderWindowIsCurrentProcessWindow())
            {
                HideForm();
                return;
            }

            if (Form.ActiveForm == this)
            {

            }
            else
            { 
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

            // 自分が先頭ではない
            IntPtr hWnd = Hm.WindowHandle;
            var list = GetWindowHidemaruHandleList();
            if (list.Count > 0 && list[0] != hWnd)
            {
                HideForm();
                return;
            }


            ShowForm();

            String strCurFileName = Hm.Edit.FilePath ?? "";
 
            // 名前があるのなら、それをナビゲート
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
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

}
