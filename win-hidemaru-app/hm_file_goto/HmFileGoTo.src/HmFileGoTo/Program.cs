using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace HmFileGoTo
{
    class Program
    {
        static void Main(string[] args)
        {
            ProcessStartInfo psi = new ProcessStartInfo();
            psi.FileName = "Hidemaru.exe";

            if (args.Length == 0 )
            {
                return;
            }

            string filename = "";
            if (args.Length >= 1) {
                filename = args[0];
            }


            int lineno = 1;
            if(args.Length >= 2)
            {
                try
                {
                    int.TryParse(args[1], out lineno);
                }
                catch (Exception) { }
            }

            int column = 0;
            if (args.Length >= 2)
            {
                try
                {
                    int.TryParse(args[2], out column);
                }
                catch (Exception) { }
            }

            string hm_fullpath = GetHideamruProgramFullPath();
            string arguments = $"/n /en /j{lineno},{column} /m3 \"{filename}\"";
            System.Diagnostics.Process.Start(hm_fullpath, arguments);
        }

        // 秀丸のパスを得る。常駐秀丸があれば、みつかる。
        static private String GetHideamruProgramFullPath()
        {
            try
            {
                // １つも起動していないし、常駐秀丸も存在しない。
                // このため、HmBaloonNotice
                var path = System.Configuration.ConfigurationManager.AppSettings["HidemaruPath"];

                // それが存在するなら、それを返す。これによって、ユーザーは秀丸以外にも結び付けることが可能だ。
                if (System.IO.File.Exists(path))
                {
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
