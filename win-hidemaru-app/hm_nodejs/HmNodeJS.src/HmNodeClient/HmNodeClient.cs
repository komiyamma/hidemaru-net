/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Runtime.Remoting.Channels.Ipc;
using System.Security.Permissions;

public partial class HmNodeClient
{
    int IP;
    string filename;

    public HmNodeClient(int IP, string filename)
    {
        this.IP = IP;
        this.filename = filename;
    }

    // プロセスをウィンドウ付きで開く
    public HmNodeClient(int IP, string filename, Options opt)
    {
        this.IP = IP;
        this.filename = filename;
        if (opt != null)
        {
            this.opt = opt;
        }
    }

    [Serializable]
    public class Options
    {
        public bool ShowConsole = false;
    }
    Options opt = new Options();

    // プロセスを開く
    public void Start()
    {
        try
        {
            var fileProcMap = HmNodeIPCServerListManager.GetExistProcessList();
            // リスト書き込み
            HmNodeIPCServerListManager.WriteProcesMap(fileProcMap);

            // 自分のプロセスだけは除去
            if (fileProcMap.Exists((m) => { return m.IP == IP; }) ) {
                return;
            }

            //Processオブジェクトを作成
            System.Diagnostics.Process proc = new System.Diagnostics.Process();

            //出力とエラーをストリームに書き込むようにする
            proc.StartInfo.FileName = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\HmNodeProxy.exe";
            string bShow = opt.ShowConsole ? "1" : "0";
            proc.StartInfo.Arguments = IP + " \"" + filename + "\" " + bShow;

            // 以下のものを確立しておかないと、Serverとのリンクが途中で切れる可能性あるので注意
            proc.StartInfo.UseShellExecute = false;

            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardError = true;
            proc.StartInfo.RedirectStandardInput = true;
            proc.OutputDataReceived += proc_OutputDataReceived;
            proc.ErrorDataReceived += proc_OutputDataReceived;

            //OutputDataReceivedとErrorDataReceivedイベントハンドラを追加

            proc.StartInfo.CreateNoWindow = true;

            //起動
            proc.Start();

            // プロセスのウィンドウが現れるまでまつ
            proc.WaitForShowWindow();

            // リストに追加
            HmNodeIPCServerListManager.AddProcessMap( new HmNodeIPCServerListManager.HmProcessStruct { IP = IP, processID = proc.Id } );

            // Console.WriteLine("実行");

        }
        catch (Exception e)
        {
            MessageBox.Show(e.Message);
        }
    }

    public void proc_OutputDataReceived(Object sender, System.Diagnostics.DataReceivedEventArgs e)
    {
        Console.WriteLine(e.Data);
    }

    public HmNodeRemoteObject.ResponseData GetResponse(System.Collections.Specialized.NameValueCollection param)
    {
        return HmNodeIPCClient.GetResponse(IP, param);
    }

    public HmNodeRemoteObject.ResponseData GetLastResponse()
    {
        return HmNodeIPCClient.GetLastResponse(IP);
    }

    // プロセスを閉じる。ちょっとバグがあるので、外からアクセスできないように
    protected void Close()
    {
        HmNodeIPCClient.UnregisterChannnel(IP);

        try
        {
            var fileProcMap = HmNodeIPCServerListManager.GetExistProcessList();

            // HmNodeProxyプロセスを列挙する
            foreach (System.Diagnostics.Process proc in System.Diagnostics.Process.GetProcessesByName("HmNodeProxy"))
            {
                // テキスト内にあるプロセスに対して…
                foreach (var pm in fileProcMap)
                {
                    // 実際に走っているプロセスのIDと一致したら…
                    if (pm.IP == IP && pm.processID == proc.Id)
                    {
                        // ウィンドウが無い場合にそなえてネイティブレベルでWM_CLOSEを送る
                        proc.SendMessageCloseWindow();

                        // 閉じるプロセスだけは除去
                        fileProcMap = fileProcMap.FindAll((m) => { return m.processID != pm.processID; });

                        // １つ除去したリストで書き込み
                        HmNodeIPCServerListManager.WriteProcesMap(fileProcMap);

                        // こちらは、IsVisibleでないと機能しない。
                        // proc.CloseMainWindow();
                        break;
                    }
                }
            }


        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }
    }
}


public partial class HmNodeClient
{

    // プロセスマップの読み込みと更新。
    static public void UpdateProcessMap()
    {
        var list = HmNodeIPCServerListManager.GetExistProcessList();
        HmNodeIPCServerListManager.WriteProcesMap(list);
    }

}