/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using Hidemaru;
using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using System;
using System.Diagnostics;



/// <summary> モードに関わらず共通 </summary>
internal partial class HmHtmlPreviewChromeForm : System.Windows.Forms.Form
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

    private void chrome_CloseCheck(object sender, EventArgs e)
    {
        try
        {
            var ret = driver.WindowHandles;
        }
        catch (Exception ex)
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
