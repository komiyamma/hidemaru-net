/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the GNU GENERAL PUBLIC LICENSE v2.0
 */

using System;

public class HmIECapt
{
    public class CheckResult
    {
        public string BuildURL = "";
        public string StatusCode = "";
        public string StatusDescription = "";
        public bool CaptureSuccess = false;
    }

    private static CheckResult TryAccessURL(String strOriginalURL)
    {

        CheckResult r = new CheckResult();

        try
        {
            UriBuilder ubBuildURL = null;
            try
            {
                r.BuildURL = strOriginalURL;
                ubBuildURL = new UriBuilder(strOriginalURL);

                //IdnMappingオブジェクトを作成する
                System.Globalization.IdnMapping im = new System.Globalization.IdnMapping();
                //punycodeにエンコードする
                string punycode = im.GetAscii(ubBuildURL.Host);

                ubBuildURL.Host = punycode;
            }
            catch (Exception err)
            {
                r.StatusCode = err.Message;
                r.StatusDescription = strOriginalURL;
                Console.WriteLine("{0}:::::{1}", r.StatusCode, r.StatusDescription);
                throw err;
            }

            //WebRequestの作成
            System.Net.HttpWebRequest webreq = (System.Net.HttpWebRequest)System.Net.WebRequest.Create(ubBuildURL.Uri);

            r.BuildURL = ubBuildURL.Uri.ToString();

            System.Net.HttpWebResponse webres = null;
            try
            {
                //サーバーからの応答を受信するためのWebResponseを取得
                webres = (System.Net.HttpWebResponse)webreq.GetResponse();

                //応答したURIを表示する
                System.Diagnostics.Trace.WriteLine(webres.ResponseUri + "\n");
                //応答ステータスコードを表示する
                System.Diagnostics.Trace.WriteLine(String.Format("{0}::{1}", webres.StatusCode, webres.StatusDescription)+ "\n");
                r.StatusCode = webres.StatusCode.ToString();
                r.StatusDescription = webres.StatusDescription.ToString();
            }
            catch (System.Net.WebException ex)
            {
                //HTTPプロトコルエラーかどうか調べる
                if (ex.Status == System.Net.WebExceptionStatus.ProtocolError)
                {
                    //HttpWebResponseを取得
                    System.Net.HttpWebResponse errres =
                        (System.Net.HttpWebResponse)ex.Response;
                    //応答したURIを表示する
                    System.Diagnostics.Trace.WriteLine(errres.ResponseUri+ "\n");
                    //応答ステータスコードを表示する
                    System.Diagnostics.Trace.WriteLine( String.Format("{0}:::{1}", errres.StatusCode, errres.StatusDescription));
                    r.StatusCode = errres.StatusCode.ToString();
                    r.StatusDescription = errres.StatusDescription.ToString();
                }
                else
                {
                    r.StatusCode = ex.ToString();
                    System.Diagnostics.Trace.WriteLine(ex.Message + "\n");
                }
            }
            finally
            {
                //閉じる
                if (webres != null)
                {
                    webres.Close();
                }
            }


        }
        catch (Exception e)
        {
            r.StatusCode = e.Message;
            r.StatusDescription = e.Message;
        }

        return r;
    }

    public static bool TryProcessCapture(string url, string outfile)
    {
        try
        {
            var selfpath = System.Reflection.Assembly.GetExecutingAssembly().Location;
            var dir = System.IO.Path.GetDirectoryName(selfpath);

            //ProcessStartInfoオブジェクトを作成する
            System.Diagnostics.ProcessStartInfo psi = new System.Diagnostics.ProcessStartInfo();
            //起動するファイルのパスを指定する
            psi.FileName = dir + "\\HmIECapt.exe";
            //コマンドライン引数を指定する
            psi.Arguments = @"--url=" + url + " " + "--out=" + "\"" + outfile + "\"";
            psi.UseShellExecute = false;
            psi.CreateNoWindow = true;
            psi.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            //アプリケーションを起動する
            System.Diagnostics.Process p = System.Diagnostics.Process.Start(psi);
            p.Start();
            p.WaitForExit(5000);

            return true;
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            return false;
        }
    }


    public static CheckResult TryCapture(string url, string outfile)
    {
        var r = TryAccessURL(url);
        if (r.StatusCode == "OK" || r.StatusDescription == "OK")
        {
            bool success = TryProcessCapture(r.BuildURL, outfile);
            r.CaptureSuccess = success;
        }
        else
        {
            r.CaptureSuccess = false;
        }

        return r;
    }
}
