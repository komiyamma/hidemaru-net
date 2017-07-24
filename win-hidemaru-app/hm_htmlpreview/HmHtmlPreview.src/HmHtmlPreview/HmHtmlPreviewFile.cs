using System;
using System.Windows.Forms;
using System.IO;
using System.Net;
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

                // 音の消去
                SetOff_IESoundEffects();

                // URLで読み直し
                Uri u = new Uri(strCurFileFullPath);
                wb.Navigate(u);

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
    protected override void update_Tick_Implements(object sender, EventArgs e)
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
    protected void watcher_Changed(object o, FileSystemEventArgs e)
    {
        try
        {
            if (e.ChangeType == System.IO.WatcherChangeTypes.Changed)
            {
                // 秀丸で編集中のテキストが変わったのだよ。
                if (string.Compare(e.FullPath, strCurFileFullPath, true) != 0)
                {

                    // 今のスクロールの位置を後で復元するので保存しておく
                    if (wb.Document != null)
                    {
                        try
                        {
                            if (wb.Document.Body != null)
                            {
                                // 手段その①．
                                // Document->Bodyが取れるパターン。これでは失敗するときもある。
                                webBrowserScroll.X = wb.Document.Body.ScrollLeft;
                                webBrowserScroll.Y = wb.Document.Body.ScrollTop;
                            }
                            // 手段その②．
                            // HTMLエレメントのScroll位置を見に行くパターン。こちらも失敗するときもある。
                            if (wb.Document.GetElementsByTagName("HTML") != null)
                            {
                                if (wb.Document.GetElementsByTagName("HTML")[0] != null)
                                {
                                    webBrowserScroll.X = wb.Document.GetElementsByTagName("HTML")[0].ScrollLeft;
                                    webBrowserScroll.Y = wb.Document.GetElementsByTagName("HTML")[0].ScrollTop;
                                }

                            }
                        }
                        catch (Exception)
                        {
                        }
                    }

                    // 音の消去
                    SetOff_IESoundEffects();
                    // URLで読み直し
                    Uri u = new Uri(strCurFileFullPath);
                    wb.Navigate(u);

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