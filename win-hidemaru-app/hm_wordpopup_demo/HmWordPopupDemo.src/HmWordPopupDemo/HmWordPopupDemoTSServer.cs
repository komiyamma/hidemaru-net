using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Windows.Forms;

using Hidemaru;

// これらはアクティブ化しないための特殊な施策。
partial class HmWordPopupDemoForm : Form
{
    private Process p;

    public void SetTSServerAttr()
    {
    }

    // プロセス確立
    private void StartTSServer()
    {
        p = new Process();

        p.StartInfo.FileName = p.StartInfo.FileName = System.Environment.GetEnvironmentVariable("ComSpec");

        p.StartInfo.CreateNoWindow = true; // コンソールを開かない
        p.StartInfo.UseShellExecute = false; // シェル機能を使用しない

        p.StartInfo.RedirectStandardInput = true;
        p.StartInfo.RedirectStandardOutput = true; // 標準出力をリダイレクト
        // ソースなのでcp932の範囲を超える事があるため。あとjsonは通常utf8形式が慣例なので
        p.StartInfo.StandardOutputEncoding = Encoding.UTF8;
        p.StartInfo.Arguments = "/c tsserver.cmd";
        p.OutputDataReceived += p_OutputDataReceived;

        p.Start(); // アプリの実行開始

        p.BeginOutputReadLine();
    }

    // プロセス停止
    private void StopTSServer()
    {
        System.Diagnostics.Trace.WriteLine("Exit TS Server");
        ExitMessage();
        p.Close();
    }

    private void _SendMessage(string request)
    {
        p.StandardInput.WriteLine(request);
    }

    int nSeqCount = 0;
    // ファイル読む
    private void OpenFileMessage(string filename)
    {
        _SendMessage("{ \"type\": \"request\", \"seq\": " + nSeqCount + ", \"command\": \"open\", \"arguments\": { \"file\": \"" + filename + "\"} }");
    }

    // ファイル更新(実態ソース内容をtmpfilenameに入れておく)
    private void ReloadFileMessage(string filename, string tmpfilename)
    {
        _SendMessage("{ \"type\": \"none\", \"seq\": " + nSeqCount + ", \"command\": \"reload\", \"arguments\": { \"file\": \"" + filename + "\", \"tmpfile\": \"" + tmpfilename + "\"} }");
    }

    // ファイル閉じる
    private void CloseFileMessage(string filename)
    {
        _SendMessage("{ \"type\": \"none\", \"seq\": " + nSeqCount + ", \"command\": \"close\", \"arguments\": { \"file\": \"" + filename + "\"} }");
    }

    // 対象の情報
    private void QuickInfoMessage(string filename, int line, int offset)
    {
        _SendMessage("{ \"type\": \"response\", \"seq\": " + nSeqCount + ", \"command\": \"quickinfo\", \"arguments\": { \"file\": \"" + filename + "\", \"line\":" + line + ", \"offset\":" + offset + "} }");
    }

    // tsserver終了
    private void ExitMessage()
    {
        _SendMessage("{ \"type\": \"none\", \"seq\": " + nSeqCount + ", \"command\": \"exit\" }");
    }

    //OutputDataReceivedイベントハンドラ
    //行が出力されるたびに呼び出される
    private void p_OutputDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
    {
        if (e.Data != null && e.Data.EndsWith("}"))
        {
            var data = e.Data.Replace("\r\r\n", "\n"); // 改行コードの修正
                                                       //出力された文字列を表示する
            AnalyzeReceivedOutputData(data);
        }
    }

    // tsserverは結果はJsonで来るため。
    [DataContract]
    class JsonData
    {
        [DataMember]
        public int @seq;

        [DataMember]
        public string @type;

        [DataMember]
        public string @command;

        [DataMember]
        public bool @success;

        [DataMember]
        public JsonDataQuickInfoBody @body;

    }

    // quickinfoの時のbody内容。他にもあるが、この程度で良いだろう。
    [DataContract]
    class JsonDataQuickInfoBody
    {
        [DataMember]
        public String @kind;

        [DataMember]
        public String kindModifiers;

        [DataMember]
        public String displayString;

        [DataMember]
        public String documentation;
    }

    // 分析
    private void AnalyzeReceivedOutputData(string recieve_data)
    {
        // Trace.WriteLine(recieve_data);

        // 必ずjson形式で標準入力に入ってくる
        var serializer = new DataContractJsonSerializer(typeof(JsonData));
        using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(recieve_data)))
        {
            var data = (JsonData)serializer.ReadObject(ms);

            // commandとしてquickinfoを要求
            if (data.command == "quickinfo")
            {
                try
                {
                    // 結果を伴うようなら
                    if (data.success)
                    {
                        // displayStringとコメント内容を併記
                        String doc = data.body.documentation;
                        lbDetail.Invoke((Action)(() => { lbDetail.Text = data.body.displayString + "\n" + data.body.documentation; }));

                    }
                    // それ以外はクリア
                    else
                    {
                        lbDetail.Invoke((Action)(() => { lbDetail.Text = ""; }));
                    }
                }
                catch (Exception e)
                {
                    Trace.WriteLine(e.Message);
                }
            }
        }
    }

    private String strPrevFileName = "";
    private String strPrevTotalText = "";
    private void tsserver_Timer_Tick(object sender, EventArgs e)
    {
        var winFilePath = Hm.Edit.FilePath;
        var winFilename = System.IO.Path.GetFileName(winFilePath);
        // ファイル名が有効である。
        if (!String.IsNullOrEmpty(winFilePath))
        {
            var strTotalText = Hm.Edit.TotalText;
            // tsserver用にUri形式にする
            Uri u = new Uri(winFilePath);
            var urlFilePath = u.AbsolutePath;
            if (strPrevFileName != urlFilePath)
            {
                strPrevFileName = urlFilePath;
                OpenFileMessage(urlFilePath);
            }

            // ソースが変化しているならば
            if (strPrevTotalText != strTotalText)
            {

                strPrevTotalText = strTotalText;

                try
                {
                    // 一時ファイルのフルパス
                    var winTempDir = System.IO.Path.GetTempPath();
                    var winTempName = winFilename + "HmWordPopupDemo.tmp";
                    var winTempPath = System.IO.Path.Combine(winTempDir, winTempName);
                    // 開いて書き込み
                    using (var sw = new StreamWriter(winTempPath))
                    {
                        sw.Write(strPrevTotalText);
                        sw.Close();
                    }
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
    }

    // 対象の要求単語(あるいは位置)が変化した場合には、tsserverにquickinfoを要求する
    private void tsserver_ChangeTargetWord(int lineno, int offset) // offset = column + 1 となる。１始まり。
    {
        if (strPrevFileName != null)
        {
            QuickInfoMessage(strPrevFileName, lineno, offset);
        }
    }

}
