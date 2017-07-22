using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Windows.Forms;

using Hidemaru;

// これらはアクティブ化しないための特殊な施策。
partial class HmTSTagJumpForm : Form
{
    private Process p;

    public static HmTSTagJumpForm form;

    public HmTSTagJumpForm()
    {
        StartTSServer();
        this.FormClosing += form_FormClosing;
    }

    private void form_FormClosing(object sender, FormClosingEventArgs e)
    {
        Stop();
    }

    private void Stop()
    {
        try
        {
            StopTSServer();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
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

        bool success = p.Start(); // アプリの実行開始

        System.Diagnostics.Trace.WriteLine("Start TS Server" + " : " + success.ToString());
        p.BeginOutputReadLine();
    }

    // プロセス停止
    private void StopTSServer()
    {
        if (p != null)
        {
            ExitMessage();
            p.Close();
            p = null;
            System.Diagnostics.Trace.WriteLine("Exit TS Server");
        }
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
    private void DefinitionMessage(string filename, int line, int offset)
    {
        _SendMessage("{ \"type\": \"response\", \"seq\": " + nSeqCount + ", \"command\": \"definition\", \"arguments\": { \"file\": \"" + filename + "\", \"line\":" + line + ", \"offset\":" + offset + "} }");
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
    class JsonDataDefinition
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
        public JsonDataDefinitionBody[] @body;

    }

    // definitionの時のbody内容。他にもあるが、この程度で良いだろう。
    [DataContract]
    class JsonDataDefinitionBody
    {
        [DataMember]
        public String @file;

        [DataMember]
        public JsonDataDefinitionBodyPos @start;

        [DataMember]
        public JsonDataDefinitionBodyPos @end;
    }

    [DataContract]
    class JsonDataDefinitionBodyPos
    {
        [DataMember]
        public int line;

        [DataMember]
        public int @offset;
    }

    // 分析
    private void AnalyzeReceivedOutputData(string recieve_data)
    {
        try
        {
            Trace.WriteLine(recieve_data);

            // 必ずjson形式で標準入力に入ってくる
            var serializer = new DataContractJsonSerializer(typeof(JsonDataDefinition));
            using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(recieve_data)))
            {
                var data = (JsonDataDefinition)serializer.ReadObject(ms);

                // commandとしてquickinfoを要求
                if (data.command == "definition")
                {
                    // 結果を伴うようなら
                    if (data.success)
                    {
                        SetResult(data.body[0]);
                    }
                    // それ以外はクリア
                    else
                    {
                    }

                    StopTSServer();
                }

            }
        }
        catch (Exception e)
        {
            Trace.WriteLine(e.Message);
        }
    }

    // 一時ファイルのフルパスを得る
    private String GetWinTempFilePath()
    {
        var winTempDir = System.IO.Path.GetTempPath();
        var winTempName = "HmTSTagJump.tmp";
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

    private void ClearResult()
    {
        Hm.Macro.Var["$RTN_filename2"] = "";
        Hm.Macro.Var["#RTN_lineno"] = -1;
        Hm.Macro.Var["#RTN_column"] = -1;
        Hm.Macro.Var["#RTN_noname"] = 0;
    }

    private void SetResult(JsonDataDefinitionBody bodyFirst)
    {
        //ローカルファイルのパスを表すURI
        string uriPath = bodyFirst.file;

        //Uriオブジェクトを作成する
        Uri u = new Uri(uriPath);

        //Windows形式のパス表現に変換する
        string winPath = u.LocalPath + Uri.UnescapeDataString(u.Fragment);

        Hm.Macro.Var["$RTN_filename2"] = winPath;
        Hm.Macro.Var["#RTN_lineno"] = bodyFirst.start.line;
        Hm.Macro.Var["#RTN_column"] = bodyFirst.start.offset - 1;  // 1始まりのoffset → 0始まりのcolumn
    }

    public void TagJump()
    {
        ClearResult();

        var pos = Hm.Edit.CursorPos;
        var lineno = pos.LineNo;
        var column = pos.Column;
        if (lineno < 0 || column < 0)
        {
            return;
        }

        var winFilePath = Hm.Edit.FilePath;
        if (winFilePath == null)
        {
            winFilePath = GetWinTempFilePath();
        }

        Hm.Macro.Var["#RTN_noname"] = 1;

        var winFilename = System.IO.Path.GetFileName(winFilePath);
        // tsserver用にUri形式にする
        Uri u = new Uri(winFilePath);
        var urlFilePath = u.AbsolutePath;
        OpenFileMessage(urlFilePath);

        var strTotalText = Hm.Edit.TotalText;

        try
        {
            var winTempPath = GetWinTempFilePath();

            WriteWinTempFilePath(strTotalText);

            // tsserverが受け付けるuri形式に
            Uri uTmp = new Uri(winTempPath);
            var urlTempFilePath = uTmp.AbsolutePath;
            // リロード
            ReloadFileMessage(urlFilePath, urlTempFilePath);

            DefinitionMessage(urlFilePath, lineno, column + 1);

            p.WaitForExit();
        }
        catch (Exception ex)
        {
            Trace.WriteLine(ex.Message);
        }

    }


}


