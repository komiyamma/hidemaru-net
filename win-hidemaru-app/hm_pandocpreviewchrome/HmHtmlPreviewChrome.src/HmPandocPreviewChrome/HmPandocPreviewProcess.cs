/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Forms;



internal partial class HmPandocPreviewChromeForm : System.Windows.Forms.Form
{
    Process pandoc;

    private void DoPandoc(string inputFileName, string outputFileName)
    {
        if (pandoc == null || pandoc.HasExited)
        {
            ; // 通過
        }
        else
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

        String filter_option = "";
        if (this.filter.Length > 0)
        {
            filter_option = " --filter " + "" + this.filter + " ";
            string task_dir = System.IO.Path.GetDirectoryName(inputFileName);

            //書き込むファイルが既に存在している場合は、上書きする
            System.IO.StreamWriter sw = new System.IO.StreamWriter(self_dir + "\\HmPandocMathFilterCurrentTaskFolder.txt");

            // 現在編集中のファイルのディレクトリ
            sw.Write(task_dir);
            //閉じる
            sw.Close();
        }


        if (mode == HmGFMPreviewMode.Github)
        {
            psi.Arguments = filter_option + "-f gfm " + "\"" + inputFileName + "\"" + " -s -t html5 -c " + "\"" + self_dir + "\\" + self_basename + ".css" + "\"" + " -o " + "\"" + outputFileName + "\" -s";

        }
        else if (mode == HmGFMPreviewMode.MathJax)
        {
            psi.Arguments = filter_option + "\"" + inputFileName + "\"" + " -s --mathjax -t html5 -c " + "\"" + self_dir + "\\" + self_basename + ".css" + "\"" + " -o " + "\"" + outputFileName + "\" -s";
            MessageBox.Show(psi.Arguments);
        }
        else if (mode == HmGFMPreviewMode.Katex)
        {
            psi.Arguments = filter_option + "\"" + inputFileName + "\"" + " -s --katex -t html5 -c " + "\"" + self_dir + "\\" + self_basename + ".css" + "\"" + " -o " + "\"" + outputFileName + "\" -s";

        }

        pandoc = Process.Start(psi);
    }

    private void StopPandoc()
    {
        try
        {
            if (pandoc != null)
            {
                pandoc.WaitForExit(300);
                pandoc.Close();
                pandoc.Kill();
            }

        }
        catch (Exception)
        {
        }
        finally
        {
            pandoc = null;
        }

    }

    // どうも Pandoc は ファイルへと出力する際に、複数回に小分けにして出すもよう。
    // そのため、watcherが１つのファイルに対して複数回反応してしまう。
    // そのため、頻繁にDoPandocしてしまわないよう、ブロッキングする。
    // 0.5秒程度ブロックしておけば十分だろう。
    private void SetPandocBlockAttribute()
    {
        pandocBlockTimer = -1;

        blocker = new Timer();
        blocker.Interval = 105;
        blocker.Tick += new EventHandler(PandocBlockTimer_Tick);
        blocker.Enabled = true;
        blocker.Start();

    }

    private void ResetPandocBlockAttribute()
    {
        pandocBlockTimer = pandocBlockTimerMax;
    }

    private const int pandocBlockTimerMax = 500;
    private int pandocBlockTimer = -1;

    private void PandocBlockTimer_Tick(object sender, EventArgs e)
    {
        pandocBlockTimer -= blocker.Interval;
    }

    //-----------------------------------------------------------------------
    // 一時ファイルとして作ったファイルのリスト。閉じた際に全部削除する。
    private List<string> tmpFileNameList = new List<string>();

    private void DeletePandocTmpFiles()
    {
        // 溜め込んでいた一時ファイルは全て削除
        foreach (var f in tmpFileNameList)
        {
            try
            {
                System.IO.File.Delete(f);
            }
            catch (Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e);
            }
        }
    }


}
