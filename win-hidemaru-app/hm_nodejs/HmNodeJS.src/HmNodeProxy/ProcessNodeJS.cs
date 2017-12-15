/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;

class ProcessNodeJS
{
    public static Process proc;


    // Nodeを起動。
    public static void Start(string filename, int IP, bool bShowWindow = false)
    {

        //Processオブジェクトを作成
        proc = new System.Diagnostics.Process();

        //出力とエラーをストリームに書き込むようにする
        proc.StartInfo.FileName = "node.exe";
        proc.StartInfo.Arguments = "\"" + filename + "\" " + IP;
        proc.StartInfo.WindowStyle = ProcessWindowStyle.Minimized;
        proc.StartInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

        if (bShowWindow)
        {
            /*
            // 以下のものを確立しておかないと、nodeとのリンクが途中で切れる可能性あるので注意
            */

            proc.StartInfo.CreateNoWindow = false;
        }
        else
        {
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.RedirectStandardInput = true;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardError = true;
            proc.OutputDataReceived += proc_OutputDataReceived;
            proc.ErrorDataReceived += proc_OutputDataReceived;

            proc.StartInfo.CreateNoWindow = true;
        }
        //起動
        proc.Start();
    }

    public static void proc_OutputDataReceived(Object sender, System.Diagnostics.DataReceivedEventArgs e)
    {
        System.Diagnostics.Debug.WriteLine(e.Data);
    }

    public static void Close()
    {
        if (proc != null)
        {
            proc.Refresh();
            try { 
                proc.StandardInput.WriteLine("query_node_terminate");
                System.Threading.Thread.Sleep(200);
            } catch
            {
                System.Diagnostics.Trace.WriteLine("NodeへのStandardInputストリームへのリダイレクトが確立されていません。\nおそらくデバッグモードで動作しています。");
            }
            try
            {
                proc.Kill();
            }
            catch (Exception /*e*/) { }
        }
    }
}


