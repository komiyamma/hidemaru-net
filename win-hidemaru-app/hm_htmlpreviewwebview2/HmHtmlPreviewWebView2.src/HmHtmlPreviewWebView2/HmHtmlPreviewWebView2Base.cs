/* 
 * Copyright (c) 2021-2021 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using Microsoft.Web.WebView2.Core;
using Microsoft.Web.WebView2.WinForms;
using System;
using System.Drawing;
using System.Windows.Forms;
using System.Threading.Tasks;
using Hidemaru;

/// やたらめったらtry_catchなのは、「いつ不意に秀丸本体が閉じても可能な限り黙って終了できるようにするため」
internal enum HmHtmlPreviewMode
{
    Live = 0,
    File = 1
}


/// <summary> モードに関わらず共通 </summary>
internal abstract partial class HmHtmlBaseForm : System.Windows.Forms.Form
{
    public static HmHtmlBaseForm form;
    protected System.IntPtr hWndHidemaru = IntPtr.Zero;
    protected string strCurFileFullPath = "";
    protected string strPrvFileFullPath = "";
    protected string strPrvHmEditTotalText = "";
    protected Timer update;
    protected WebView2 wb;

    public HmHtmlBaseForm(System.IntPtr hWndHidemaru)
    {
        this.hWndHidemaru = hWndHidemaru;

        SetFormAttribute();
        SetWebBrowserAttribute();
        SetTimerAttribute();
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

    /// <summary>フォームの属性設定</summary>
    protected void SetFormAttribute()
    {
        this.Text = GetStartTitle();

        // このフォームサイズ
        this.Width = 800;
        this.Height = 600;

        // 効果なさそうだが、一応Waitカーソルは使わない
        Application.UseWaitCursor = false;

        // 閉じる直前
        this.FormClosing += new FormClosingEventHandler(form_FormClosing);
    }

    /// <summary>Webブラウザ属性設定</summary>
    protected void SetWebBrowserAttribute()
    {
        // WebBrowserオブジェクトを配置
        wb = wb = new WebView2
        {
            Source = new Uri(GetStartTitle())
        };
        wb.Anchor = (AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right | AnchorStyles.Bottom);
        wb.NavigationCompleted += wb_NavigationCompleted;
        wb.ClientSize = this.ClientSize;

        // スクリプトエラーでもダイアログとか出さない
        // wb.ScriptErrorsSuppressed = true;

        // フォームのサイズ変化した時追従
        this.SizeChanged += new EventHandler(form_SizeChanged);
        this.Controls.Add(wb);

        wb_DocumentInit();
    }


    /// <summary>タイマー属性設定。データ更新の必要性があるかどうかの基礎更新。
    /// Liveモードでは、このタイミングがデータ更新のタイミング。
    /// Fileモードでは、このタイミングはユーザーが「なんのファイル」を開いているのか、の情報の更新のみ。
    ///</summary>
    protected void SetTimerAttribute()
    {
        // １秒に１回の更新で十分だろう。
        update = new Timer();
        update.Interval = 1000 * 1;
        update.Tick += new EventHandler(update_Tick);
        update.Start();

        update_Tick(null, null);
    }

    protected async void update_Tick(object sender, EventArgs e)
    {
        update_Tick_Implements(sender, e);
    }

    protected abstract void update_Tick_Implements(object sender, EventArgs e);

    //=========================================================================================
    // フォーム
    //=========================================================================================
    protected void form_SizeChanged(object o, EventArgs e)
    {
        try
        {
            // WebBrowserをフォーム全体に。
            wb.ClientSize = this.ClientSize;
        }
        catch (Exception)
        {
        }
    }


    private void form_FormClosing(object o, FormClosingEventArgs e)
    {
        try
        {
            //監視を終了
            Stop();

            // ガベージコレクトもしておく。
            GC.Collect();
        }
        catch (Exception)
        {
        }
    }

    // wbはドキュメントが更新されると、ページ位置が強制的にページトップへと移動してしまう。
    // よってドキュメントを更新する前に、座標を保存しておき、ドキュメント更新後に、元へと戻すため
    protected Point webBrowserScroll = new Point(0, 0);

    // 文章内容が更新した、ということを手動で。
    protected bool isDocumentChanged = false;


    protected abstract void wb_DocumentInit();

    public virtual void Stop()
    {
        try
        {
            if (update != null)
            {
                // フォームを閉じた時も解放。
                update.Stop();
                update = null;
            }
        }
        catch (Exception)
        {
        }
    }


    /// <summary> wbのドキュメントの更新が完全に完了した時(最初の読み込み時も完了するとここに来る) </summary>
    protected async void wb_NavigationCompleted(object sender, CoreWebView2NavigationCompletedEventArgs e)
    {
        await ScrollToPreviousPotision();
    }

    private async Task ScrollToPreviousPotision()
    {
        try
        {
            if (isDocumentChanged)
            {
                isDocumentChanged = false;
                // 保持していた座標へとスクロール
                await wb.ExecuteScriptAsync($"window.scrollTo({webBrowserScroll.X}, {webBrowserScroll.Y})");

                try
                {
                    // Document->Bodyが取れるパターン。これでは失敗するときもある。
                    int X = int.Parse(await wb.ExecuteScriptAsync("document.body.scrollLeft"));
                    int Y = int.Parse(await wb.ExecuteScriptAsync("document.body.scrollTop"));

                    if (X != webBrowserScroll.X || Y != webBrowserScroll.Y)
                    {
                        await Task.Delay(300);
                        await wb.ExecuteScriptAsync($"window.scrollTo({webBrowserScroll.X}, {webBrowserScroll.Y})");
                    }
                }
                catch (Exception)
                {
                }


            }
        }
        catch (Exception)
        {
        }
    }

}
