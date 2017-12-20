/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using Hidemaru;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;



internal partial class HmPandocPreviewChromeForm : System.Windows.Forms.Form
{
    public static HmPandocPreviewChromeForm form; // 自分自身

    public enum HmGFMPreviewMode
    {
        Pandoc = 0,
        Github = 1,
        MathJax = 2,
        Katex = 3
    }
    protected HmGFMPreviewMode mode = new HmGFMPreviewMode();

    protected String filter;

    protected System.IntPtr hWndHidemaru = IntPtr.Zero;
    protected string strCurFileFullPath = "";
    protected string strPrvFileFullPath = "";
    protected string strCurFileFullPathTmp = "";

    protected Timer update;
    protected Timer blocker;

    public HmPandocPreviewChromeForm(System.IntPtr hWndHidemaru, HmGFMPreviewMode mode, String filter)
    {
        this.hWndHidemaru = hWndHidemaru;
        this.mode = mode;
        this.filter = filter;

        SetFormAttribute();
        SetWebBrowserAttribute();
        SetPandocBlockAttribute();
        SetTimerAttribute();
    }

    /// <summary>フォームの属性設定</summary>
    private void SetFormAttribute()
    {

        // 効果なさそうだが、一応Waitカーソルは使わない
        Application.UseWaitCursor = false;

        isFirstWather = true;
    }

    private void SetTimerAttribute()
    {
        // １秒に１回の更新で十分だろう。
        update = new Timer();
        update.Interval = 1000 * 1;
        update.Tick += new EventHandler(update_Tick);
        update.Tick += new EventHandler(chrome_CloseCheck);
        update.Tick += new EventHandler(chrome_PageEndAdjustCheck);
        update.Enabled = true;
        update.Start();
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

    string GetTmpHtmlFileNameFromMarkDownFileName(string stdMDFileName)
    {
        if (String.IsNullOrEmpty(stdMDFileName))
        {
            return "";
        }

        var directory = Path.GetDirectoryName(stdMDFileName);

        var basename = Path.GetFileNameWithoutExtension(stdMDFileName);

        basename += ".tmp.html";

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
            DeletePandocTmpFiles();

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

            StopSelenium();

            StopPandoc();

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
                    if (pandocBlockTimer <= 0)
                    {
                        RefleshBrowserPage();
                        ResetPandocBlockAttribute();
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

        // 最初の１回だけはファイルの生成を待つ
        if (isFirstWather) {
            // pandoc.WaitForExit();
            isFirstWather = false;
        }


        // ここを通過するということはファイルのフルパスが変更となったのであろうから、Urlの更新
        UpdateBrowserUrl(strCurFileFullPathTmp);
    }
}
