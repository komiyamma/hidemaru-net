using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace HmRipGrepTest
{
    static class RipGrep
    {

        static void Main(string[] args)
        {
            //Processオブジェクトを作成する
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            //起動するファイルを指定する
            p.StartInfo.FileName = "rg.exe";

            List<String> list = new List<String>();
            list.Add("-n");
            list.Add("-e");
            list.Add(Regex.Escape("Jav."));
            list.Add("-S");
            list.Add(@"C:\usr\web");

            String line = list.EncodeCommandLineValues();
            Console.WriteLine(line);
            p.StartInfo.Arguments = list.EncodeCommandLineValues();

            p.StartInfo.UseShellExecute = false;
            //イベントハンドラの追加
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.RedirectStandardOutput = true;

            p.ErrorDataReceived += P_ErrorDataReceived;
            p.OutputDataReceived += P_OutputDataReceived;

            p.Exited += p_Exited;
            //プロセスが終了したときに Exited イベントを発生させる
            p.EnableRaisingEvents = true;
            //起動する
            p.Start();

            p.BeginOutputReadLine();

            p.WaitForExit();
        }

        private static void P_OutputDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            Console.WriteLine(e.Data);
        }

        private static void P_ErrorDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            Console.WriteLine(e.Data);
        }

        private static void p_Exited(object sender, EventArgs e)
        {
            Console.WriteLine("終わり");
        }
    }

    static class StringCommandLineExtension
    {
        /// 
        /// コマンドライン引数 1 個をエンコード
        /// 
        public static string EncodeCommandLineValue(this string value)
        {
            if (string.IsNullOrEmpty(value)) return "";
            var containsSpace = value.IndexOfAny(new[] { ' ', '\t' }) != -1;

            // 「\…\"」をエスケープ
            // やってることは、「"」直前の「\」の数を 2倍+1
            value = _commandLineEscapePattern.Replace(value, @"$1\$&");

            // スペース／タブが含まれる場合はデリミタで囲み、末尾が「\」だった場合、エスケープ
            if (containsSpace)
            {
                value = "\"" + _lastBackSlashPattern.Replace(value, "$1$1") + "\"";
            }
            return value;
        }
        private static readonly Regex _commandLineEscapePattern = new Regex("(\\\\*)\"");
        private static readonly Regex _lastBackSlashPattern = new Regex(@"(\\ + )$");

        /// 
        /// コマンドライン引数複数個をエンコードして、スペースで結合
        /// 
        public static string EncodeCommandLineValues(this IEnumerable<string> values)
        {
            if (values == null) throw new ArgumentNullException("values");
            return string.Join(" ", values.Select(v => EncodeCommandLineValue(v)));
        }
    }


}





// Regex.Escape
// 正規表現を含んだ文字を、ただの文字列へとエスケープする


//　以下は、コマンドライン引数としてダメな文字列をエスケープする
/*
/// 
/// コマンドライン引数 1 個をエンコード
/// 
public static string EncodeCommandLineValue(this string value)
{
	if (string.IsNullOrEmpty(value)) return "";
	var containsSpace = value.IndexOfAny(new[] {' ', '\t'}) != -1;

	// 「\…\"」をエスケープ
	// やってることは、「"」直前の「\」の数を 2倍+1
	value = _commandLineEscapePattern.Replace(value, @"$1\$&");

		// スペース／タブが含まれる場合はデリミタで囲み、末尾が「\」だった場合、エスケープ
		if (containsSpace) {
			value = "\"" + _lastBackSlashPattern.Replace(value, "$1$1") + "\"";
		}
	return value;
}
private static readonly Regex _commandLineEscapePattern = new Regex("(\\\\*)\"");
private static readonly Regex _lastBackSlashPattern = new Regex(@"(\\ + )$");

/// 
/// コマンドライン引数複数個をエンコードして、スペースで結合
/// 
public static string EncodeCommandLineValues(this IEnumerable<string> values)
{
	if (values == null) throw new ArgumentNullException("values");
	return string.Join(" ", values.Select(v = > EncodeCommandLineValue(v)));
}
*/

