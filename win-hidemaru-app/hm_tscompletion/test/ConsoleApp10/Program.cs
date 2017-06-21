using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System;
using System.IO;
using System.IO.Pipes;
using System.Diagnostics;
using System.Windows.Forms;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;

/*
 * "seq":0,"type":"response","command":"unkn
essage":"Error processing request. Unexpe
*/
namespace LanguageServerProtocol
{
    class WinForm : Form
    {
        Button btn;
        TextBox tb;
        public WinForm()
        {
            this.FormClosing += WinForm_FormClosing;
            _StartServer();
            InitControl();
            _ReaderLoop();
        }

        Process p;
        private void _StartServer()
        {
            p = new Process();

            p.StartInfo.FileName = p.StartInfo.FileName = System.Environment.GetEnvironmentVariable("ComSpec");

            p.StartInfo.CreateNoWindow = false; // コンソールを開かない
            p.StartInfo.UseShellExecute = false; // シェル機能を使用しない

            p.StartInfo.RedirectStandardInput = true;
            p.StartInfo.RedirectStandardOutput = true; // 標準出力をリダイレクト
            p.StartInfo.Arguments = "/c tsserver.cmd";
            p.OutputDataReceived += p_OutputDataReceived;

            p.Start(); // アプリの実行開始
        }

        private void _ReaderLoop()
        {
            // string output = p.StandardOutput.ReadToEnd(); // 標準出力の読み取り
            p.BeginOutputReadLine();
        }

        private void InitControl()
        {
            btn = new Button() { Left = 10, Top = 10, Text = "ボタン" };
            tb = new TextBox() { Left = 10, Top = 100, Height = 100 };
            tb.Multiline = true;
            btn.Click += Btn_Click;
            this.Controls.Add(btn);
            this.Controls.Add(tb);
        }

        private void Btn_Click(object sender, EventArgs e)
        {
            if (p != null)
            {
                _SendMessage(tb.Text);
            }
        }

        private void WinForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (p != null)
            {
                p.Close();
            }

        }

        private void _SendMessage(string request)
        {
            p.StandardInput.WriteLine(request);
        }

        //OutputDataReceivedイベントハンドラ
        //行が出力されるたびに呼び出される
        private static void p_OutputDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            if (e.Data != null && e.Data.EndsWith("}"))
            {
                var data = e.Data.Replace("\r\r\n", "\n"); // 改行コードの修正
                                                             //出力された文字列を表示する
                _ReadMessage(data);
            }
        }

        private static void _ReadMessage(string recieve_data)
        {
            Console.WriteLine("★");
            Console.WriteLine(recieve_data);

            var serializer = new DataContractJsonSerializer(typeof(JsonData));
            using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(recieve_data)))
            {
                var data = (JsonData)serializer.ReadObject(ms);
                Console.WriteLine(data.type);
            }
        }
    }


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
        public string @message;
    }

    class TypeSriptPipeServer
    {
        public TypeSriptPipeServer()
        {

        }

        static public void abc()
        {
            new WinForm().ShowDialog();

            return;
        }

    }

    class Program
    {
        static void Main(string[] args)
        {
            LanguageServerProtocol.TypeSriptPipeServer.abc();
        }
    }
}
