using Hidemaru;
using OpenQA.Selenium;
using OpenQA.Selenium.Chrome;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;



/// <summary> モードに関わらず共通 </summary>
internal class HmPandocPreviewChromeForm : System.Windows.Forms.Form
{

    public enum HmGFMPreviewMode
    {
        Github = 0,
        MathJax = 1
    }
    protected HmGFMPreviewMode mode = new HmGFMPreviewMode();

    public static HmPandocPreviewChromeForm form;
    protected System.IntPtr hWndHidemaru = IntPtr.Zero;
    protected string strCurFileFullPath = "";
    protected string strPrvFileFullPath = "";
    protected string strCurFileFullPathTmp = "";
    protected string strPrvFileFullPathTmp = "";
    protected string strPrvHmEditTotalText = "";

    private List<string> tmpFileNameList = new List<string>();

    protected Timer update;
    protected Timer blocker;

    protected IWebDriver driver;
    protected ChromeOptions chromeOptions;
    protected ChromeDriverService chromeService;

    public HmPandocPreviewChromeForm(System.IntPtr hWndHidemaru, IntPtr pmode)
    {
        this.hWndHidemaru = hWndHidemaru;
        mode = (HmGFMPreviewMode)pmode;

        SetFormAttribute();
        SetWebBrowserAttribute();
        SetTimerAttribute();
    }

    /// <summary>フォームの属性設定</summary>
    private void SetFormAttribute()
    {

        // 効果なさそうだが、一応Waitカーソルは使わない
        Application.UseWaitCursor = false;

        isFirstWather = true;
    }

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

    private void SetTimerAttribute()
    {
        // １秒に１回の更新で十分だろう。
        update = new Timer();
        update.Interval = 1000 * 1;
        update.Tick += new EventHandler(update_Tick);
        update.Tick += new EventHandler(chrome_CloseCheck);
        update.Enabled = true;
        update.Start();

        blocker = new Timer();
        blocker.Interval = 200;
        blocker.Tick += new EventHandler(blocker_Tick);
        blocker.Enabled = true;
        blocker.Start();

    }

    private const int nextRefleshBlockTimeDefault = 500;
    private int nextRefleshBlockTime = -1;
    private void blocker_Tick(object sender, EventArgs e)
    {
        nextRefleshBlockTime -= blocker.Interval;
    }


    private void update_Tick(object sender, EventArgs e)
    {
        try
        {
            strCurFileFullPath = Hm.Edit.FilePath ?? "";
            strCurFileFullPathTmp = GetTmpHtmlFileNameFromMarkDownFileName(strCurFileFullPath);
            if (strCurFileFullPath.Length > 0)
            {
                if (strPrvFileFullPath != strCurFileFullPath)
                {
                    strPrvFileFullPath = strCurFileFullPath;
                    SetWatcherAttribute();
                }
            }
        }
        catch (Exception ex)
        {
            Trace.WriteLine(ex.Message);
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


    string GetTmpHtmlFileNameFromMarkDownFileName(string stdMDFileName)
    {
        if (String.IsNullOrEmpty(stdMDFileName))
        {
            return "";
        }

        var directory = Path.GetDirectoryName(stdMDFileName);

        var basename = Path.GetFileNameWithoutExtension(stdMDFileName);

        basename += "_tmp_for_preview.html";

        var fullpath = Path.Combine(directory, basename);

        // 未登録ならば登録。(後で削除するため)
        if (!tmpFileNameList.Contains(fullpath))
        {
            tmpFileNameList.Add(fullpath);
        }

        return fullpath;
    }

    public void Stop()
    {
        try
        {
            // 溜め込んでいた一時ファイルは全て削除
            foreach(var f in tmpFileNameList)
            {
                try
                {
                    System.IO.File.Delete(f);
                } catch(Exception e)
                {
                    System.Diagnostics.Trace.WriteLine(e);
                }
            }

            if (update != null)
            {
                // フォームを閉じた時も解放。
                update.Stop();
                update = null;
            }
            if (blocker != null)
            {
                blocker.Stop();
                blocker = null;
            }

            // watcherを特別に追加してるのでそれも止める
            if (watcher != null)
            {
                //監視を終了
                watcher.EnableRaisingEvents = false;
                watcher = null;
            }
            if (driver != null)
            {
                driver.Close();
                driver.Quit();
            }

            if (pandoc != null)
            {
                pandoc.WaitForExit();
                pandoc.Close();
                pandoc = null;
            }

        }
        catch (Exception)
        {
        }
    }

    /// <summary>ファイルの監視</summary>
    private System.IO.FileSystemWatcher watcher = null;

    /// <summary>ファイルの監視の属性。
    /// 秀丸がファイル名を変更したり、ディレクトリを変更したりしても追跡できるようにするため。
    /// 又、ファイルの保存時は、webBrowserの内容を更新する。
    /// </summary>
    private void SetWatcherAttribute()
    {
        try
        {
            if (strCurFileFullPath.Length > 0)
            {
                if (watcher == null)
                {
                    watcher = new System.IO.FileSystemWatcher();
                    watcher.NotifyFilter = (System.IO.NotifyFilters.LastAccess | System.IO.NotifyFilters.LastWrite | System.IO.NotifyFilters.FileName | System.IO.NotifyFilters.DirectoryName);
                }

                watcher.Path = System.IO.Path.GetDirectoryName(strCurFileFullPath);
                // wbとは内容更新が非同期衝突するので、そこは同期せよ
                watcher.Filter = "*.*";

                watcher.Changed += new FileSystemEventHandler(watcher_Changed);
                watcher.IncludeSubdirectories = false;

                //監視を開始するb
                watcher.EnableRaisingEvents = true;

                watcher_Renamed(strCurFileFullPath);
            }
        }
        catch (Exception)
        {
        }
    }

    Process pandoc;

    private void DoPandoc(string inputFileName, string outputFileName)
    {
        if (pandoc == null || pandoc.HasExited)
        {
            ; // 通過
        } else
        {
            return;
        }

        // mathjaxのパターン: pandoc a.md -s --mathjax -o b.html -s
        // githubのパターン:pandoc a.md -s -t html5 -c github.css -o b.html

        System.Diagnostics.ProcessStartInfo psi = new System.Diagnostics.ProcessStartInfo();
        psi.FileName = "pandoc.exe";
        psi.CreateNoWindow = true;
        psi.UseShellExecute = false;

        string self_full_path = System.Reflection.Assembly.GetExecutingAssembly().Location;
        string self_dir = System.IO.Path.GetDirectoryName(self_full_path);
        string self_basename = System.IO.Path.GetFileNameWithoutExtension(self_full_path);

        // モードによって分ける
        if (mode == HmGFMPreviewMode.Github)
        {
            psi.Arguments = "-f gfm " + "\"" + inputFileName + "\"" + " -s -t html5 -c " + "\"" + self_dir + "\\" + self_basename +".css" + "\"" + " -o " + "\"" + outputFileName + "\" -s";

        } else if (mode == HmGFMPreviewMode.MathJax) { 
            psi.Arguments = "\"" + inputFileName + "\"" + " -s --mathjax -t html5 -c " + "\"" + self_dir + "\\" + self_basename + ".css" + "\"" + " -o " + "\"" + outputFileName + "\" -s";

        }

        pandoc = Process.Start(psi);
    }



    /// <summary>ファイルの監視の属性。
    /// 秀丸がファイル名を変更したり、ディレクトリを変更したりしても追跡できるようにするため。
    /// 又、ファイルの保存時は、webBrowserの内容を更新する。
    /// </summary>
    private void watcher_Changed(object o, FileSystemEventArgs e)
    {
        try
        {
            if (e.ChangeType == System.IO.WatcherChangeTypes.Changed)
            {
                var strOpenFileFullPath = Hm.Edit.FilePath ?? "";

                // 今、秀丸で編集中のmdと一致
                if (String.Compare(e.FullPath, strOpenFileFullPath, true) == 0)
                {
                    strCurFileFullPathTmp = GetTmpHtmlFileNameFromMarkDownFileName(strOpenFileFullPath);
                    DoPandoc(strOpenFileFullPath, strCurFileFullPathTmp);
                }

                // 今、変換後のhtmlファイル名と一緒
                else if ( String.Compare(e.FullPath, strCurFileFullPathTmp, true) == 0 )
                {
                    if (nextRefleshBlockTime <= 0)
                    {
                        driver.Navigate().Refresh();
                        nextRefleshBlockTime = nextRefleshBlockTimeDefault;
                    }
                }
            }
        }
        catch (Exception)
        {
        }
    }

    private bool isFirstWather = true;
    private void watcher_Renamed(string strCurFileFullPath)
    {
        strCurFileFullPathTmp = GetTmpHtmlFileNameFromMarkDownFileName(strCurFileFullPath);
        DoPandoc(strCurFileFullPath, strCurFileFullPathTmp);
        if (isFirstWather) {
            pandoc.WaitForExit();
            isFirstWather = false;
        }


        // ここを通過するということはファイルのフルパスが変更となったのであろうから、Urlの更新
        driver.Url = strCurFileFullPathTmp;
    }
}
