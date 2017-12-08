/*
 * Copyright (c) 2015-2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace HmBaloonNotice
{
    public partial class HmBaloonNoticeForm : Form
    {
        String m_message;
        Timer time = new Timer();
        NotifyIcon notifyIconInfo;

        public const int showtime = 5000;

        public HmBaloonNoticeForm(String message, int showtime)
        {
            m_message = message;

            notifyIconInfo = new NotifyIcon();
            //タスクトレイにアイコンを表示するための設定
            //一度設定すれば、バルーンヒントを表示するたびに設定しなおす必要はない
            //NotifyIconのアイコンを設定する
            notifyIconInfo.Icon = SystemIcons.Information;
            //NotifyIconを表示する
            notifyIconInfo.Visible = true;

            //バルーンヒントの設定
            //バルーンヒントのタイトル
            notifyIconInfo.BalloonTipTitle = m_message;
            //バルーンヒントに表示するメッセージ
            notifyIconInfo.BalloonTipText = m_message;
            //バルーンヒントに表示するアイコン
            notifyIconInfo.BalloonTipIcon = ToolTipIcon.Info;
            //バルーンヒントを表示する
            //表示する時間をミリ秒で指定する
            notifyIconInfo.ShowBalloonTip(showtime);
            notifyIconInfo.BalloonTipClicked += new EventHandler(notifyIconInfo_BalloonTipClicked);

            InitializeComponent();

            // タイマーの作成
            time = new Timer();
            time.Enabled = true;
            time.Interval = showtime;
            time.Tick += new EventHandler(timer_Tick);
        }

        // 1回きりの実行。
        private void timer_Tick(Object o, EventArgs e)
        {
            notifyIconInfo.Visible = false;
            time.Enabled = false;
            this.Close();
            Application.Exit();
        }

        private void notifyIconInfo_BalloonTipClicked(Object o, EventArgs e)
        {
            try
            {
                // 引数の文字列はクリックした時秀丸を起動するべきかどうか見当する
                String message = m_message;

                // 受け取ったも文字列がタグジャンプの形となっているかどうか。
                message.Replace("/", @"\");
                Regex rgx = new Regex(@"^(.+)?:(\d+)");
                Match m = rgx.Match(message);
                String file = m.Groups[1].Value;
                Console.WriteLine(file);
                String line = m.Groups[2].Value;

                // タグジャンプの形になっていなければ、全体をファイルとして仮定する。
                if (file == "")
                {
                    file = message;
                }

                // 常駐秀丸があれば、hideamruというプロセスが１つは存在する。
                String hm_fullpath = GetHideamruProgramFullPath();

                // 秀丸が見つかった＆受け取ったメッセージのファイルは実在する
                if (hm_fullpath.Length > 0 && System.IO.File.Exists(file))
                {
                    // タグジャンプ形式の場合
                    if (line != "")
                    {
                        System.Diagnostics.Process.Start(hm_fullpath, " /m3 /j" + line + " " + '"' + file + '"');
                    }
                    // ファイルだけの場合
                    else
                    {
                        System.Diagnostics.Process.Start(hm_fullpath, '"' + file + '"');
                    }
                }


            }
            catch (Exception err)
            {
                System.Diagnostics.Trace.WriteLine(err);
            }
        }

        // 秀丸のパスを得る。常駐秀丸があれば、みつかる。
        private String GetHideamruProgramFullPath()
        {
            try
            {
                // １つも起動していないし、常駐秀丸も存在しない。
                // このため、HmBaloonNotice
                var path = System.Configuration.ConfigurationManager.AppSettings["HidemaruPath"];

                // それが存在するなら、それを返す。これによって、ユーザーは秀丸以外にも結び付けることが可能だ。
                if (System.IO.File.Exists(path)) { 
                    return path;
                }

                // ここまで来たということは、該当のexeは存在していないということ。(正しい設定をしていないのか)

                // 以下はサブモード。常駐秀丸や、別の秀丸エディタが起動していることを期待して、秀丸のフルパスを求める

                // hidemaru のプロセスを取得
                System.Diagnostics.Process[] ps = System.Diagnostics.Process.GetProcessesByName("hidemaru");

                foreach (System.Diagnostics.Process p in ps)
                {
                    // 見つかったらフルパスを返す
                    return p.MainModule.FileName;
                }
            }
            catch (Exception err)
            {
                System.Diagnostics.Trace.WriteLine(err);
            }

            return "";

        }
    }

}
