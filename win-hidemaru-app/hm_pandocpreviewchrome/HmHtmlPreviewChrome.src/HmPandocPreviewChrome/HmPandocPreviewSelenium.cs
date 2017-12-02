using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using System;
using System.Diagnostics;
using Hidemaru;


internal partial class HmPandocPreviewChromeForm : System.Windows.Forms.Form
{

    protected IWebDriver driver;
    protected ChromeOptions chromeOptions;
    protected ChromeDriverService chromeService;


    /// <summary>Webブラウザ属性設定</summary>
    private void SetWebBrowserAttribute()
    {
        update_Tick(null, null);

        try
        {
            chromeOptions = new ChromeOptions();
            string self_full_path = System.Reflection.Assembly.GetExecutingAssembly().Location;
            string self_dir = System.IO.Path.GetDirectoryName(self_full_path);

            chromeService = ChromeDriverService.CreateDefaultService(self_dir);
            chromeService.HideCommandPromptWindow = true;

            driver = new ChromeDriver(chromeService, chromeOptions);
            watcher_Renamed(strCurFileFullPath);
        }
        catch (Exception e)
        {
            Trace.WriteLine(e.Message);
        }
    }

    private void chrome_PageEndAdjustCheck(object sender, EventArgs e)
    {
        // System.Diagnostics.Trace.WriteLine("chrome_PageEndCheck");

        try
        {
            // 全体のテキストの改行の数
            string strTotalText = Hm.Edit.TotalText;
            int LineCount = strTotalText.Split('\n').Length;

            int LineNo = Hm.Edit.CursorPos.LineNo;

            double dCursorLineRatio = 0.0;

            // ゼロ除算防止とともに、無駄にfCursorPositionRatioが1になるのも防止
            if (LineCount >= 1)
            {
                dCursorLineRatio = (double)LineNo / (double)LineCount;
            }

            // 最終付近にカーソルがあるならば…
            if (dCursorLineRatio > 0.95)
            {

                // var executor = driver as IJavaScriptExecutor;
                IJavaScriptExecutor executor = driver as IJavaScriptExecutor;

                if (mode == HmGFMPreviewMode.Github)
                {
                    // JavaScriptとして評価
                    executor.ExecuteScript(
                    @"
                    function __getScrollBottom() {
                      var body = window.document.body;
                      var html = window.document.documentElement;
                      var scrollTop = body.scrollTop || html.scrollTop;
                      return html.scrollHeight - html.clientHeight - scrollTop;
                    }
                    if (__getScrollBottom() < 70) {
                        window.scrollBy(0,70);
                    }
                    ", "");

                }

                // MathJaxはかなり描画が乱れるので、秀丸上で最後らへんにいるのであれば、比較的ひろめに吸着範囲を取っておく
                else if(mode == HmGFMPreviewMode.MathJax)
                {
                    // JavaScriptとして評価
                    executor.ExecuteScript(
                    @"
                    function __getScrollBottom() {
                      var body = window.document.body;
                      var html = window.document.documentElement;
                      var scrollTop = body.scrollTop || html.scrollTop;
                      return html.scrollHeight - html.clientHeight - scrollTop;
                    }
                    if (__getScrollBottom() < 300) {
                        window.scrollBy(0,300);
                    }
                    ", "");

                }


            }
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex);
        }
    }

    private void chrome_CloseCheck(object sender, EventArgs e)
    {
        try
        {
            var ret = driver.WindowHandles;
        }
        catch (Exception)
        {
            System.Diagnostics.Trace.WriteLine("クローズしている");
            this.Stop();
            this.Close();
        }
    }

    private void RefleshBrowserPage()
    {
        driver.Navigate().Refresh();
    }

    private void UpdateBrowserUrl(string fullpath)
    {
        driver.Url = fullpath;
    }

    private void StopSelenium()
    {
        try
        {
            if (driver != null)
            {
                driver.Close();
                driver.Quit();
            }
        }
        catch (Exception)
        {
        }
    }

}
