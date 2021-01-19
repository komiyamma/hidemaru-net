﻿/* 
 * Copyright (c) 2021-2021 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using Hidemaru;
using System;
using System.IO;

internal partial class HmHtmlLiveForm : HmHtmlBaseForm
{
    public HmHtmlLiveForm(System.IntPtr hWndHidemaru) : base(hWndHidemaru)
    {
    }

    /// <summary>フォーム表示時にWebBrowserに流す情報。
    /// Liveモードの時は、Tickでやってることと同じことをする。
    ///</summary>
    protected override void wb_DocumentInit()
    {
        try
        {
            update_Tick(null, null);
        }
        catch (Exception)
        {
        }
    }

    /// <summary>Liveモード用のタイマーイベント
    /// 座標を保存、相対ディレクトリの内容を絶対ディレクトリへと修正、音を消して、テキストを更新する。
    /// テキストが更新された後は、webBrowser_Compo
    ///</summary>
    protected override async void update_Tick_Implements(object sender, EventArgs e)
    {
        try
        {
            strCurFileFullPath = Hm.Edit.FilePath ?? "";

            // 現在の編集の内容を取得
            string curHmEditTotalText = Hm.Edit.TotalText;


            // テキストが変化した時、もしくは、ファイル名が変化した時に更新
            if (strPrvHmEditTotalText != curHmEditTotalText || strCurFileFullPath != strPrvFileFullPath)
            {
                // タイトルが食い違ってる場合は更新
                if (strCurFileFullPath != strPrvFileFullPath)
                {
                    this.Text = strCurFileFullPath;
                }

                strPrvFileFullPath = strCurFileFullPath;
                strPrvHmEditTotalText = curHmEditTotalText;

                if (true)
                {
                    // 手段その①．
                    // Document->Bodyが取れるパターン。これでは失敗するときもある。
                    webBrowserScroll.X = int.Parse( await wb.ExecuteScriptAsync("document.body.scrollLeft") );
                    webBrowserScroll.Y = int.Parse( await wb.ExecuteScriptAsync("document.body.scrollTop") );

                    // 手段その②．
                    // HTMLエレメントのScroll位置を見に行くパターン。こちらも失敗するときもある。
                    if (webBrowserScroll.Y == 0)
                    {
                        webBrowserScroll.X = int.Parse(await wb.ExecuteScriptAsync(@"document.getElementsByTagName('HTML')[0].scrollLeft") );
                        webBrowserScroll.Y = int.Parse(await wb.ExecuteScriptAsync(@"document.getElementsByTagName('HTML')[0].scrollTop"));
                    }
                }

                // ファイル名が有効であるならば、相対を変換する
                if (strCurFileFullPath.Length > 0)
                {
                    // 相対→絶対
                    curHmEditTotalText = RelativeToAbsolute(curHmEditTotalText);
                }


                // テキスト更新
                wb.NavigateToString(curHmEditTotalText);
                isDocumentChanged = true;
            }
        }
        catch (Exception)
        {
        }


    }


    /// <summary>
    /// 事実上Liveモードのみ通過する。
    /// 実体のないファイルの場合、画像やリンクは相対アドレスは不可能なので、絶対アドレスへと置き換える。
    /// </summary>
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
                HtmlAgilityPack.HtmlNode nodeBase = HtmlAgilityPack.HtmlNode.CreateNode("<base href=''>");
                nodeBase.Attributes["href"].Value = basedir + "\\";

                // Headタグがあればそこにたす
                HtmlAgilityPack.HtmlNode head = doc.DocumentNode.SelectSingleNode("/html/head");
                if (head != null)
                {
                    head.PrependChild(nodeBase);
                }
                // Headタグがないなら、トップにたさざるをえないだろう
                else
                {
                    doc.DocumentNode.PrependChild(nodeBase);
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
