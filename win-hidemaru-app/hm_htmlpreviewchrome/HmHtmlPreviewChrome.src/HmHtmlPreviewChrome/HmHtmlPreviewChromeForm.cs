/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using Hidemaru;
using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;



/// <summary> モードに関わらず共通 </summary>
internal partial class HmHtmlPreviewChromeForm : System.Windows.Forms.Form
{

    public static HmHtmlPreviewChromeForm form;

    /// やたらめったらtry_catchなのは、「いつ不意に秀丸本体が閉じても可能な限り黙って終了できるようにするため」
    public enum HmHtmlPreviewMode
    {
        Live = 0,
        File = 1
    }

    protected HmHtmlPreviewMode mode = new HmHtmlPreviewMode();

    protected System.IntPtr hWndHidemaru = IntPtr.Zero;
    protected string strCurFileFullPath = "";
    protected string strPrvFileFullPath = "";

    protected Timer update;

    public HmHtmlPreviewChromeForm(System.IntPtr hWndHidemaru)
    {
        this.hWndHidemaru = hWndHidemaru;

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

    private void SetTimerAttribute()
    {
        // １秒に１回の更新で十分だろう。
        update = new Timer();
        update.Interval = 1000 * 1;
        update.Tick += new EventHandler(update_Tick);
        update.Tick += new EventHandler(chrome_CloseCheck);
        update.Enabled = true;
        update.Start();
    }

    private void update_Tick(object sender, EventArgs e)
    {
        try
        {
            strCurFileFullPath = Hm.Edit.FilePath ?? "";
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

    public void Stop()
    {
        try
        {
            if (update != null)
            {
                // フォームを閉じた時も解放。
                update.Stop();
                update = null;
            }
            // watcherを特別に追加してるのでそれも止める
            if (watcher != null)
            {
                //監視を終了
                watcher.EnableRaisingEvents = false;
                watcher = null;
            }

            StopSelenium();
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

                //監視を開始する
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

                // 今、秀丸で編集中のテキストファイル名と一致
                if ( String.Compare(e.FullPath, strOpenFileFullPath, true) == 0 )
                {
                    RefleshBrowserPage();
                }
            }
        }
        catch (Exception)
        {
        }
    }

    private void watcher_Renamed(string strCurFileFullPath)
    {
        // ここを通過するということはファイルのフルパスが変更となったのであろうから、Urlの更新
        UpdateBrowserUrl(strCurFileFullPath);
    }
}
