using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Forms;

using Hidemaru;

internal sealed class HmTextSynChromeForm : System.Windows.Forms.Form
{
    public static HmTextSynChromeForm form;

    private IntPtr hWndHidemaru = IntPtr.Zero;
    private string strPrvTotalText = "";
    private string strOption1 = "--lang=ja";
    private Timer update;

    private IWebDriver driver;
    private ChromeOptions chromeOptions;
    private ChromeDriverService chromeService;
    private bool is_driver_ready = false;


    public HmTextSynChromeForm(System.IntPtr hWndHidemaru, string strOption1)
    {
        this.hWndHidemaru = hWndHidemaru;
        this.strOption1 = strOption1;

        SetFormAttribute();
        SetWebBrowserAttribute();
        SetTimerAttribute();
    }


    /// <summary>フォームの属性設定</summary>
    private void SetFormAttribute()
    {

        // 効果なさそうだが、一応Waitカーソルは使わない
        Application.UseWaitCursor = false;
    }

    /// <summary>Webブラウザ属性設定</summary>
    private void SetWebBrowserAttribute()
    {

        try
        {
            chromeOptions = new ChromeOptions();
            chromeOptions.AddArgument(strOption1);
            chromeService = ChromeDriverService.CreateDefaultService();
            chromeService.HideCommandPromptWindow = true;

            driver = new ChromeDriver(chromeService, chromeOptions);
            driver.Url = "about:blank";
            is_driver_ready = true;
        }
        catch (Exception e)
        {
            Debug.WriteLine(e.Message);
        }
    }

    private void SetTimerAttribute()
    {
        // １秒に１回の更新で十分だろう。
        update = new System.Windows.Forms.Timer();
        update.Interval = 1000 * 1;
        update.Tick += update_Tick;
        update.Tick += new EventHandler(chrome_CloseCheck);
        update.Start();

        update_Tick(null, null);
    }


    private void TaskMethod()
    {
        string curTotalText = Hm.Edit.TotalText;
        if (curTotalText != strPrvTotalText)
        {
            strPrvTotalText = curTotalText;

            var body = driver.FindElement(By.TagName("body"));

            // var executor = driver as IJavaScriptExecutor;
            IJavaScriptExecutor executor = driver as IJavaScriptExecutor;


            // まず、JavaScriptによるEvaluateの予定なので、それを邪魔する要素は置き換え
            string encodeText = curTotalText.Replace(@"\", @"\\");
            encodeText = encodeText.Replace("'", @"\'").Replace(@"""", @"\""");

            // 改行を糾合
            encodeText = encodeText.Replace("\r\n", "\r");
            encodeText = encodeText.Replace("\n", "\r");
            encodeText = encodeText.Replace("\r", @"\n");

            // JavaScriptとして評価
            executor.ExecuteScript("arguments[0].innerText = '" + encodeText + "'", body);
        }
    }

    private List<Task> TaskList = new List<Task>();
    private void update_Tick(object sender, EventArgs e)
    {
        if (!is_driver_ready)
        {
            return;
        }
        try
        {
            // 非同期で結果を秀丸へと反映するためのタスクを生成
            Task task = Task.Factory.StartNew(new Action(TaskMethod));

            // リストの中で終わってるタスクはクリア
            for (int ix = TaskList.Count - 1; ix >= 0; ix--)
            {
                if (TaskList[ix].IsCompleted)
                {
                    TaskList.RemoveAt(ix);
                }
            }

            // 改めてタスクに加える
            TaskList.Add(task);
        }
        catch (Exception)
        {
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

    public void Stop()
    {

        Task.WaitAll(TaskList.ToArray());
        try
        {
            if (update != null)
            {
                // フォームを閉じた時も解放。
                update.Stop();
                update = null;
            }
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
