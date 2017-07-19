using Hidemaru;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;

internal partial class HmMarkDownPreviewForm : System.Windows.Forms.Form
{
    public static HmMarkDownPreviewForm form;
    private System.IntPtr hWndHidemaru = IntPtr.Zero;
    private String strCurFileFullPath = "";
    private String strPrvFileFullPath = "";
    private List<String> strHmEditTotalTextQueue = new List<string>();
    private bool isFirstTickDone = false;
    public SplitContainer sc;

    public Timer update;
    public WebBrowser wb;
    public RichTextBox tb;


    public HmMarkDownPreviewForm(System.IntPtr hWndHidemaru)
    {
        this.hWndHidemaru = hWndHidemaru;

        SetFormAttribute();
        SetTextBoxAttribute();
        SetWebBrowserAttribulte();
        SetTimerAttribute();
    }

    public void SetSplitContainer()
    {
        sc = new SplitContainer();
        sc.BorderStyle = BorderStyle.FixedSingle;
        this.Controls.Add(sc);
        sc.Anchor = (AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right | AnchorStyles.Bottom);

        sc.SplitterMoved += new SplitterEventHandler(sc_SplitterMoved);
    }

    public void SetFormAttribute()
    {

        SetSplitContainer();

        this.Text = "秀丸用 HmMarkDownPreview";

        // このフォームサイズ
        this.Width = 600;
        this.Height = 600;
        sc.ClientSize = this.ClientSize;

        // 効果なさそうだが、一応Waitカーソルは使わない
        Application.UseWaitCursor = false;

        // 閉じる直前
        this.FormClosing += new FormClosingEventHandler(form_FormClosing);
    }

    public void SetTextBoxAttribute()
    {
        tb = new RichTextBox();
        tb.Multiline = true;
        tb.ScrollBars = RichTextBoxScrollBars.Vertical;
        sc.Panel1.Controls.Add(tb);
        tb.ClientSize = sc.Panel1.ClientSize;
    }

    public void SetWebBrowserAttribulte()
    {
        // WebBrowserオブジェクトを配置
        wb = new WebBrowser();
        wb.Anchor = (AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right | AnchorStyles.Bottom);
        wb.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(wb_DocumentCompleted);

        // 右側に配置。目いっぱい
        wb.ClientSize = sc.Panel2.ClientSize;

        // スクリプトエラーでもダイアログとか出さない
        wb.ScriptErrorsSuppressed = true;

        // フォームのサイズ変化した時追従
        this.SizeChanged += new EventHandler(form_SizeChanged);
        sc.Panel2.Controls.Add(wb);
    }

    public void SetTimerAttribute()
    {
        // １秒に１回の更新で十分だろう。
        update = new Timer();
        update.Interval = 500 * 1;
        update.Tick += new EventHandler(update_Tick);
        update.Start();
        update.Enabled = true;

        update_Tick(null, null);
    }

    private void form_SizeChanged(object o, EventArgs e)
    {
        try
        {
            sc.ClientSize = this.ClientSize;

            // 左側のサイズ調整
            tb.ClientSize = sc.Panel1.ClientSize;

            // WebBrowserを右側全体に。
            wb.ClientSize = sc.Panel2.ClientSize;

        }
        catch (Exception)
        {
        }
    }

    private void sc_SplitterMoved(object o, SplitterEventArgs e)
    {
        form_SizeChanged(null, null);
    }

    private void form_FormClosing(object o, FormClosingEventArgs e)
    {
        try
        {
            // フォームを閉じた時も解放。
            update.Stop();
            update = null;

            // ガベージコレクトもしておく。
            GC.Collect();
        }
        catch (Exception)
        {
        }
    }

    private Point webBrowserScroll = new Point(0, 0);
    private bool isDocumentChanged = false;

    private void update_Tick(object sender, EventArgs e)
    {
        try
        {
            string curHmEditTotalText = Hm.Edit.TotalText;

            strCurFileFullPath = Hm.Edit.FilePath ?? "";

            strHmEditTotalTextQueue.Add(curHmEditTotalText);
            if (strHmEditTotalTextQueue.Count > 3)
            {
                strHmEditTotalTextQueue.RemoveAt(0);
            }

            // テキストが変化した時、もしくは、ファイル名が変化した時に更
            // 履歴が３つあり、
            if ((strHmEditTotalTextQueue.Count >= 3 &&
               strHmEditTotalTextQueue[0] != strHmEditTotalTextQueue[1] && // 過去に食い違いがあるのに…
               strHmEditTotalTextQueue[1] == strHmEditTotalTextQueue[2]) // 今同じ
               || strCurFileFullPath != strPrvFileFullPath || !isFirstTickDone) // ファイル名が違う
            {
                isFirstTickDone = true;
                strPrvFileFullPath = strCurFileFullPath;

                Markdown md = new Markdown();

                // 現在の編集の内容を取得
                string curHmHTMLTotalText = md.Transform(curHmEditTotalText);

                if (wb.Document != null)
                {
                    // 手段その①．
                    // Document->Bodyが取れるパターン。これでは失敗するときもある。
                    webBrowserScroll.X = wb.Document.Body.ScrollLeft;
                    webBrowserScroll.Y = wb.Document.Body.ScrollTop;

                    // 手段その②．
                    // HTMLエレメントのScroll位置を見に行くパターン。こちらも失敗するときもある。
                    if (webBrowserScroll.Y == 0)
                    {
                        webBrowserScroll.X = wb.Document.GetElementsByTagName("HTML")[0].ScrollLeft;
                        webBrowserScroll.Y = wb.Document.GetElementsByTagName("HTML")[0].ScrollTop;
                    }
                }

                // テキスト更新
                tb.Text = curHmHTMLTotalText;

                // ファイル名が有効であるならば、相対を変換する
                if (strCurFileFullPath.Length > 0)
                {
                    // 相対→絶対
                    curHmHTMLTotalText = RelativeToAbsolute(curHmHTMLTotalText);
                }

                // 音の消去
                SetOff_IESoundEffects();

                // テキスト更新
                wb.DocumentText = curHmHTMLTotalText;
                isDocumentChanged = true;
            }
        }
        catch (Exception err)
        {
            Debug.WriteLine(err.Message + "\n");
        }
    }

    // テキスト更新が完了した時(最初の読み込み時もここに来る)
    private void wb_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
    {
        try
        {
            if (isDocumentChanged)
            {
                isDocumentChanged = false;

                // 音を戻す
                SetOn_IESoundEffects();

                // 保持していた座標へとスクロール
                wb.Document.Window.ScrollTo(webBrowserScroll.X, webBrowserScroll.Y);
            }
        }
        catch (Exception)
        {
        }
    }

    //=========================================================================================
    // 実体のないファイルの場合、画像やリンクは相対アドレスは不可能なので、絶対アドレスへと置き換える。
    //=========================================================================================
    private string RelativeToAbsolute(string html)
    {

        try
        {
            string fn = strCurFileFullPath;
            fn = fn.Replace("\\", "/");
            string strBaseUrl = fn;
            HtmlAgilityPack.HtmlDocument doc = new HtmlAgilityPack.HtmlDocument();
            doc.LoadHtml(html);

            bool isExistBaseHref = false;
            foreach (var nodeBase in doc.DocumentNode.Descendants("base"))
            {
                if (nodeBase.Attributes["href"].Value.Length > 0)
                {
                    isExistBaseHref = true;
                }
                else
                {

                }
            }

            // base hrefの指定が無いのであれば、現在の開いているファイルのディレクトリをそこにしておくことで、相対ディレクトリをフォローする。
            if (!isExistBaseHref)
            {
                string basedir = System.IO.Path.GetDirectoryName(strBaseUrl);
                HtmlAgilityPack.HtmlNode baseNode = HtmlAgilityPack.HtmlNode.CreateNode("<base href=''>");
                baseNode.Attributes["href"].Value = basedir + "\\";

                // Headタグがあればそこにたす
                HtmlAgilityPack.HtmlNode nodeHead = doc.DocumentNode.SelectSingleNode("/html/head");
                HtmlAgilityPack.HtmlNode nodeHtml = doc.DocumentNode.SelectSingleNode("/html");
                if (nodeHead != null)
                {
                    nodeHead.PrependChild(baseNode);
                }
                else if (nodeHtml != null)
                {
                    nodeHtml.PrependChild(baseNode);
                }
                // Headタグがないなら、トップにたさざるをえないだろう
                else
                {
                    doc.DocumentNode.PrependChild(baseNode);
                }
            }

            StringWriter writer = new StringWriter();
            doc.Save(writer);

            string newHtml = writer.ToString();
            return newHtml;
        }
        catch (Exception)
        {
        }

        return html;
    }

}
