using System;
using System.Runtime.Remoting.Channels.Ipc;
using System.Security.Permissions;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Threading;
using System.IO;


/*
// Remote object.
public class HidemaruNodeRemoteObject : MarshalByRefObject
{
    private int callCount = 0;

    public const string Name = "HidemaruNodeRemoteObject.rem";

    public int GetCount()
    {
        callCount++;
        Console.WriteLine("{0} 回メソッドが呼ばれました。", callCount);
        return (callCount);
    }

    /// 自動的に切断されるのを回避
    public override object InitializeLifetimeService()
    {
        return null;
    }
}
*/


public class HmNodeIPCServer
{
    static IpcChannel serverChannel;

    [SecurityPermission(SecurityAction.Demand)]
    public static void Main(string[] args)
    {
        // 引数が２つ必要。１番目のIPで、２番目がnodeに引き渡す.jsのファイル名
        if (args.Length < 3)
        {
            return;
        }

        // 常駐秀丸以外が何も起動していなければ、何もしない。(hideの秀丸も起動しているものとする)
        if (!HidemaruWindowInfo.IsFindWindow())
        {
            return;
        }

        try
        {
            // １番めの引数にはIPが入っている50000～65535の範囲あたりを想定
            int IP = int.Parse(args[0]);

            bool bShowWindow = int.Parse(args[2]) > 0; // 1ならtrue, 0ならfalse

            // IPCサーバーチャンネルの生成.
            serverChannel = new IpcChannel("localhost:" + IP);

            // リモート用として、サーバーチャンネルを登録
            System.Runtime.Remoting.Channels.ChannelServices.RegisterChannel(serverChannel, false);

            // チャンネル名の表示.
            Console.WriteLine("チャンネルの名前は {0}.",
                serverChannel.ChannelName);

            // チャンネルの優先度.
            Console.WriteLine("チャンネルの優先度は {0}.", serverChannel.ChannelPriority);

            // 該当のチャンネルデータの取得.
            System.Runtime.Remoting.Channels.ChannelDataStore channelData = (System.Runtime.Remoting.Channels.ChannelDataStore)serverChannel.ChannelData;
            // チャンネルの場所表示
            foreach (string uri in channelData.ChannelUris)
            {
                Console.WriteLine("チャンネルのURIは {0}.", uri);
            }

            // リモート呼び出しのため、オブジェクトを登録し、公開。サーバータイプ。シングルトン。
            System.Runtime.Remoting.RemotingConfiguration.RegisterWellKnownServiceType(
                    typeof(HmNodeRemoteObject), HmNodeRemoteObject.Name + IP + ".rem",
                    System.Runtime.Remoting.WellKnownObjectMode.Singleton
            );

            // チャンネルの URI を分析.
            string[] urls = serverChannel.GetUrlsForUri(HmNodeRemoteObject.Name + IP + ".rem");
            if (urls.Length > 0)
            {
                string objectUrl = urls[0];
                string objectUri;
                string channelUri = serverChannel.Parse(objectUrl, out objectUri);
                Console.WriteLine("オブジェクトの URI は {0}.", objectUri);
                Console.WriteLine("チェンネルの URI は {0}.", channelUri);
                Console.WriteLine("オブジェクトの URL は {0}.", objectUrl);
            }

            // ユーザーの入力待ち
            // Console.WriteLine("サーバー終了には、リターンキーを入力");


            // Node.JSを、ファイル名やIPを指定して起動
            ProcessNodeJS.Start(args[1], IP, bShowWindow);

            // ShowDialogにすることで、一種モーダルとする。(常駐秀丸以外の)秀丸が無ければ、Close()してくる。
            // 外アプリからClose()した時に、正当な処理が行われるように(nodeもCloseできるように)、Formにしている。
            var f = new HmNodeProxyForm(IP, bShowWindow);
            f.ShowDialog();

            // Node.JSの終了
            ProcessNodeJS.Close();

            // チャンネルの解除
            System.Runtime.Remoting.Channels.ChannelServices.UnregisterChannel(serverChannel);

            HmNodeIPCServerListManager.UpdateProcessMap();

            // Console.WriteLine("サーバーを終了します。");
        }
        catch (Exception e)
        {
            MessageBox.Show(e.Message + "ううう");
        }
    }

}



public class HmNodeIPCServerListManager
{
    private static Mutex mutex = new Mutex(false, "HmNodeJSExistProcessListFileMutex");

    public struct HmProcessStruct
    {
        public int IP;
        public int processID;
    }

    static string mapfilename = System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location) + @"\HmNodeProxy.txt";

    public static List<HmProcessStruct> GetExistProcessList()
    {

        List<int> existProcList = new List<int>();
        List<HmProcessStruct> fileProcMap = new List<HmProcessStruct>();

        //全てのプロセスを列挙する
        foreach (System.Diagnostics.Process p in System.Diagnostics.Process.GetProcessesByName("HmNodeProxy"))
        {
            // Console.WriteLine("◎{0}", p.Id);
            existProcList.Add(p.Id);
        }

        // 実際にファイルに存在するポートIDとプロセスIDのペアを読み込んでいく
        // 存在するプロセスIDのものだけ残してリスト化しておく


        System.IO.FileStream fsr = new System.IO.FileStream(
        mapfilename,
        System.IO.FileMode.OpenOrCreate,
        System.IO.FileAccess.Read,
        System.IO.FileShare.None);

        //----------------------------------------------
        // ミューテックスロック開始
        //----------------------------------------------
        mutex.WaitOne();

        using (StreamReader sr = new StreamReader(fsr))
        {

            //Regexオブジェクトを作成
            System.Text.RegularExpressions.Regex r = new System.Text.RegularExpressions.Regex(@"(\d+) (\d+)");

            while (sr.Peek() > -1)
            {
                var line = sr.ReadLine();

                //TextBox1.Text内で正規表現と一致する対象を1つ検索
                System.Text.RegularExpressions.Match m = r.Match(line);

                if (m.Success)
                {

                    int IP = int.Parse(m.Groups[1].Value);
                    int processID = int.Parse(m.Groups[2].Value);

                    if (existProcList.Contains(processID))
                    {
                        //                           Console.WriteLine("■プロセス{0} {1}", IP, processID);
                        fileProcMap.Add(new HmProcessStruct { IP = IP, processID = processID });
                    }

                }
            }
        }

        //----------------------------------------------
        // ミューテックスロック解除
        //----------------------------------------------
        mutex.ReleaseMutex();

        return fileProcMap;
    }

    public static void WriteProcesMap(List<HmProcessStruct> fileProcMap)
    {
        // 現在実際に存在したプロセスのものだけをリスト化した状態でファイルに書き出しておく

        System.IO.FileStream fsw = new System.IO.FileStream(
        mapfilename,
        System.IO.FileMode.Create,
        System.IO.FileAccess.Write,
        System.IO.FileShare.None);

        //----------------------------------------------
        // ミューテックスロック開始
        //----------------------------------------------
        mutex.WaitOne();

        using (StreamWriter sw = new StreamWriter(fsw))
        {
            if (fileProcMap.Count == 0)
            {
                Console.Write("");
            }
            foreach (var pm in fileProcMap)
            {
                //                    Console.WriteLine("◎プロセス{0} {1}", pm.IP, pm.processID);
                sw.WriteLine("{0} {1}", pm.IP, pm.processID);
            }
        }

        //----------------------------------------------
        // ミューテックスロック解除
        //----------------------------------------------
        mutex.ReleaseMutex();
    }

    // プロセスマップの読み込みと更新。
    public static void UpdateProcessMap()
    {
        try
        {
            var fileProcMap = GetExistProcessList();
            // 現在実行中のプロセス
            var currentprocess = System.Diagnostics.Process.GetCurrentProcess();

            // 自分のプロセスだけは除去
            fileProcMap = fileProcMap.FindAll((m) => { return m.processID != currentprocess.Id; });

            // リスト書き込み
            WriteProcesMap(fileProcMap);
        }
        catch (Exception /*e*/)
        {

        }
    }

    // プロセスマップに「ポートID」と「プロセスID」のペアを１つ分追加。
    public static void AddProcessMap(HmProcessStruct p)
    {
        int IP = p.IP;
        int processID = p.processID;
        try
        {
            System.IO.FileStream fsw = new System.IO.FileStream(
            mapfilename,
            System.IO.FileMode.Append,
            System.IO.FileAccess.Write,
            System.IO.FileShare.ReadWrite);

            //----------------------------------------------
            // ミューテックスロック開始
            //----------------------------------------------
            mutex.WaitOne();

            using (StreamWriter sw = new StreamWriter(fsw))
            {
                sw.WriteLine("{0} {1}", IP, processID);
            }

            //----------------------------------------------
            // ミューテックスロック解除
            //----------------------------------------------
            mutex.ReleaseMutex();
        }
        catch (Exception /*e*/)
        {

        }
    }

}