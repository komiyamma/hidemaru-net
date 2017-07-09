using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Reflection;
using System.Text.RegularExpressions;




internal static class StringEncodeCommadnExtension
{
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
    private static readonly Regex _lastBackSlashPattern = new Regex(@"(\\+)$");

    /// 
    /// コマンドライン引数複数個をエンコードして、スペースで結合
    /// 
    public static string EncodeCommandLineValues(this IEnumerable<string> values)
    {
        if (values == null) throw new ArgumentNullException("values");
        return string.Join(" ", values.Select(v => EncodeCommandLineValue(v)));
    }
}

internal class RipGrepCommandLine
{
    System.Diagnostics.Process p;
    Dictionary<String, bool> hs;
    String searcText = "";
    String dirText = "";

    bool m_isContinueMode = false;
    public RipGrepCommandLine(String searcText, String dirText, Dictionary<String, bool> prev)
    {
        this.searcText = searcText;
        this.dirText = dirText;

        // まだ辞書が全くなりなら
        if (prev == null)
        {
            hs = new Dictionary<String, Boolean>();
            m_isContinueMode = false;
        }
        else
        {
            hs = prev;
            m_isContinueMode = true;
        }
    }

    public Dictionary<String, bool> Grep()
    {
        try
        {
            //Processオブジェクトを作成する
            p = new Process();
            //起動するファイルを指定する
            var self = Assembly.GetExecutingAssembly().Location;

            p.StartInfo.FileName = System.IO.Path.GetDirectoryName(self) + @"\rg.exe";

            List<String> list = new List<String>();
            if (m_isContinueMode)
            {
                list.Add("-E");
                list.Add("sjis");
            }
            list.Add("--no-ignore");
            list.Add("-n");
            list.Add("-e");
            list.Add(Regex.Escape(this.searcText));
            list.Add("-S");
            list.Add(this.dirText);

            string arg_line = list.EncodeCommandLineValues();
            p.StartInfo.Arguments = arg_line;

            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.UseShellExecute = false;

            //イベントハンドラの追加
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.RedirectStandardOutput = true;

            p.ErrorDataReceived += proc_ErrorDataReceived;
            p.OutputDataReceived += proc_OutputDataReceived;
            p.StartInfo.StandardOutputEncoding = Encoding.UTF8;
            p.StartInfo.StandardErrorEncoding = Encoding.UTF8;

            //プロセスが終了したときに Exited イベントを発生させる
            // p.EnableRaisingEvents = true;

            //起動する
            p.Start();
            p.BeginOutputReadLine();

            p.WaitForExit();

            try
            {
                if (p != null)
                {
                    p.Close();
                    p.Kill();
                }
            }
            catch 
            {

            }
        }
        catch (Exception e)
        {
            Trace.WriteLine(e.Message);
        }

        return hs;
    }

    private Regex r = new Regex(@"^[\s\S]+?:\d+:");

    private void proc_OutputDataReceived(object sender, DataReceivedEventArgs e)
    {
        String data = e.Data;
        if (data == null)
        {
            return;
        }

        try
        {
            string s = r.Match(data).Value;
            if (s != null)
            {
                bool is_must_add = false;
                if (m_isContinueMode)
                {
                    // 今度は、まだ登録されていない時だけ、SJIS版を吐き出す
                    if (!hs.ContainsKey(s))
                    {
                        is_must_add = true;
                    }
                }
                else
                {
                    hs[s] = true;
                    is_must_add = true;
                }

                if (is_must_add)
                {
                    Console.WriteLine(data);
                }

            }
        }
        catch (Exception ex)
        {
            Trace.WriteLine(ex.Message);
        }

    }
    private void proc_ErrorDataReceived(object sender, DataReceivedEventArgs e)
    {
        proc_OutputDataReceived(sender, e);
    }
}

public class IronRG
{
    public static void Main(String[] args)
    {
        RipGrepCommandLine rgcl1 = new RipGrepCommandLine(args[0], args[1], null);
        var dic1 = rgcl1.Grep();

        RipGrepCommandLine rgcl2 = new RipGrepCommandLine(args[0], args[1], dic1);
        rgcl2.Grep();
    }
}