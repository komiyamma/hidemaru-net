/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Windows.Forms;
using System.IO;

using Hidemaru;

internal class HmHtmlFileForm : HmHtmlBaseForm
{
    public HmHtmlFileForm(System.IntPtr hWndHidemaru) : base(hWndHidemaru)
    {
    }

    /// <summary>フォーム表示時にWebBrowserに流す情報。
    /// Fileモードの時は、無音でUriの内容を流し込む
    ///</summary>
    protected override void wb_DocumentInit()
    {
        try
        {
            strCurFileFullPath = Hm.Edit.FilePath ?? "";

            // 秀丸で編集中のテキストが変わったのだよ。
            if (strCurFileFullPath.Length > 0)
            {


                // URLで読み直し
                Uri u = new Uri(strCurFileFullPath);
                wb.Source = u;

                if (strPrvFileFullPath != strCurFileFullPath)
                {
                    strPrvFileFullPath = strCurFileFullPath;
                    SetWatcherAttribute();
                }
            }
        }
        catch (Exception)
        {
        }
    }

    /// <summary>タイマー属性設定。データ更新の必要性があるかどうかの基礎更新。
    /// Fileモードでは、このタイミングはユーザーが「なんのファイル」を開いているのか、の情報の更新。
    /// 前回と食い違っていたら、監視対象のフォルダ等を再度設定し直す
    ///</summary>
    protected override async void update_Tick_Implements(object sender, EventArgs e)
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
            else
            {
                Stop();
            }
        }
        catch (Exception)
        {
        }
    }



    /// <summary>Fileモード用のファイルの監視</summary>
    protected System.IO.FileSystemWatcher watcher = null;

    /// <summary>Fileモード用のファイルの監視の属性。
    /// 秀丸がファイル名を変更したり、ディレクトリを変更したりしても追跡できるようにするため。
    /// 又、ファイルの保存時は、webBrowserの内容を更新する。
    /// </summary>
    protected void SetWatcherAttribute()
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
                watcher.SynchronizingObject = wb;
                watcher.Filter = "*.*";

                watcher.Changed += new FileSystemEventHandler(watcher_Changed);
                watcher.IncludeSubdirectories = false;

                //監視を開始する
                watcher.EnableRaisingEvents = true;
            }
        }
        catch (Exception)
        {
        }
    }


    /// <summary>Fileモード用のファイルの監視の属性。
    /// 秀丸がファイル名を変更したり、ディレクトリを変更したりしても追跡できるようにするため。
    /// 又、ファイルの保存時は、webBrowserの内容を更新する。
    /// </summary>
    protected async void watcher_Changed(object o, FileSystemEventArgs e)
    {
        try
        {
            if (e.ChangeType == System.IO.WatcherChangeTypes.Changed)
            {
                var strOpenFileFullPath = Hm.Edit.FilePath ?? "";

                // 今、秀丸で編集中のテキストファイル名と一致
                if (String.Compare(e.FullPath, strOpenFileFullPath, true) == 0)
                {
                    if (true)
                    {
                        // 手段その①．
                        // Document->Bodyが取れるパターン。これでは失敗するときもある。
                        webBrowserScroll.X = int.Parse(await wb.ExecuteScriptAsync("document.body.scrollLeft"));
                        System.Diagnostics.Trace.WriteLine(webBrowserScroll.X);
                        webBrowserScroll.Y = int.Parse(await wb.ExecuteScriptAsync("document.body.scrollTop"));
                        System.Diagnostics.Trace.WriteLine(webBrowserScroll.Y);

                        // 手段その②．
                        // HTMLエレメントのScroll位置を見に行くパターン。こちらも失敗するときもある。
                        if (webBrowserScroll.Y == 0)
                        {
                            webBrowserScroll.X = int.Parse(await wb.ExecuteScriptAsync(@"document.getElementsByTagName('HTML')[0].scrollLeft"));
                            webBrowserScroll.Y = int.Parse(await wb.ExecuteScriptAsync(@"document.getElementsByTagName('HTML')[0].scrollTop"));
                        }
                    }

                    // URLで読み直し
                    Uri u = new Uri(strCurFileFullPath);
                    wb.Source = u;
                    wb.NavigateToString(Hm.Edit.TotalText);
                    isDocumentChanged = true;

                }
            }
        }
        catch (Exception)
        {
        }
    }

    /// <summary>Fileモード用のファイルの監視の終了。
    /// ファイルが新規になっている時(ファイルの実態が無い時)や、フォームが終了する直前、などに停止される。
    /// </summary>
    public override void Stop()
    {
        try
        {
            // 親の分はやっとく
            base.Stop();

            // watcherを特別に追加してるのでそれも止める
            if (watcher != null)
            {
                //監視を終了
                watcher.EnableRaisingEvents = false;
                watcher = null;
            }
        }
        catch (Exception)
        {
        }
    }

}