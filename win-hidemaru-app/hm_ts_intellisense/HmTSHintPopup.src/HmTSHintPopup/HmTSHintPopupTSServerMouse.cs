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

// これらはアクティブ化しないための特殊な施策。
partial class HmTSHintPopupForm : Form
{
    private String strPrevFileName = "";

    private List<String> strHmEditTotalTextQueue = new List<string>();
    private int ListCapacitySize = 4;

    // 一時ファイルのフルパスを得る
    private String GetWinTempFilePath()
    {
        var winTempDir = System.IO.Path.GetTempPath();
        var winTempName = "HmTSHintPopup.tmp";
        var winTempPath = System.IO.Path.Combine(winTempDir, winTempName);
        return winTempPath;
    }

    // 一時ファイルへとtextを書き込み
    private void WriteWinTempFilePath(String text)
    {
        var winTempPath = GetWinTempFilePath();
        // 開いて書き込み
        using (var sw = new StreamWriter(winTempPath))
        {
            sw.Write(text);
            sw.Close();
        }
    }

    private void tsserver_Timer_Tick(object sender, EventArgs e)
    {
        if (p == null)
        {
            return;
        }
        if (p.HasExited)
        {
            Trace.WriteLine("TS Server は未起動");
            return;
        }

        var winFilePath = Hm.Edit.FilePath;
        if (winFilePath == null)
        {
            winFilePath = GetWinTempFilePath();
        }
        var winFilename = System.IO.Path.GetFileName(winFilePath);

        // tsserver用にUri形式にする
        Uri u = new Uri(winFilePath);
        var urlFilePath = u.AbsolutePath;
        if (strPrevFileName != urlFilePath)
        {
            strPrevFileName = urlFilePath;
            OpenFileMessage(urlFilePath);
        }

        var strTotalText = Hm.Edit.TotalText;

        strHmEditTotalTextQueue.Add(strTotalText);
        if (strHmEditTotalTextQueue.Count > ListCapacitySize)
        {
            strHmEditTotalTextQueue.RemoveAt(0);
        }

        // テキストが変化する度に伝えていては頻繁すぎる。
        // 「４つの履歴のうち、１番最初の履歴文字列だけが２番目以降とは異なる」とすることで、
        // 入力が一段落したことの証となる。
        if ((strHmEditTotalTextQueue.Count == 1) || (strHmEditTotalTextQueue.Count >= ListCapacitySize &&
            strHmEditTotalTextQueue[0] != strHmEditTotalTextQueue[1] && // 過去に食い違いがあるのに…
            strHmEditTotalTextQueue[1] == strHmEditTotalTextQueue[2] &&
            strHmEditTotalTextQueue[2] == strHmEditTotalTextQueue[3]
            )) // 今同じ
        {

            try
            {
                var winTempPath = GetWinTempFilePath();

                WriteWinTempFilePath(strTotalText);

                // tsserverが受け付けるuri形式に
                Uri uTmp = new Uri(winTempPath);
                var urlTempFilePath = uTmp.AbsolutePath;
                // リロード
                ReloadFileMessage(urlFilePath, urlTempFilePath);
            }
            catch (Exception ex)
            {
                Trace.WriteLine(ex.Message);
            }

        }
    }

    // 対象の要求単語(あるいは位置)が変化した場合には、tsserverにquickinfoを要求する
    private void tsserver_ChangeTargetWord(int lineno, int offset) // offset = column + 1 となる。１始まり。
    {
        if (p == null)
        {
            return;
        }
        if (p.HasExited)
        {
            Trace.WriteLine("TS Server は未起動");
            return;
        }

        if (strPrevFileName != null)
        {
            QuickInfoMessage(strPrevFileName, lineno, offset);
        }
    }

}
