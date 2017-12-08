/*
 * Copyright (c) 2015-2017 Akitsugu Komiyama
 * under the MIT License
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HmBaloonNotice
{
    static class Program
    {
        /// <summary>
        /// アプリケーションのメイン エントリ ポイントです。
        /// </summary>
        [STAThread]
        static void Main(String[] args)
        {
            if (args.Length < 1)
            {
                MessageBox.Show("使い方:\n    HmBaloonNotice.exe  \"ファイルのフルパス\":指定行  \"表示タイム(ミリ秒)\"");
                return;
            }

            //二重起動をチェックする
            if (System.Diagnostics.Process.GetProcessesByName(
                    System.Diagnostics.Process.GetCurrentProcess().ProcessName).Length > 1
                ) {
                return;
            }

            // １番目はメッセージ
            string message = args[0];
            int showtime = HmBaloonNoticeForm.showtime;
            try
            {
                // ２番目は表示をミリ秒で
                if (args.Length >= 2) {
                    showtime = int.Parse(args[1]);
                }
            }
            catch (Exception e) { }

            // Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            HmBaloonNoticeForm f = new HmBaloonNoticeForm(message, showtime);
            Application.Run();
        }
    }
}
