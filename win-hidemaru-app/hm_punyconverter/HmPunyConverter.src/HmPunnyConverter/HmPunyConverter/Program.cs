using System;
using System.Reflection;
using System.Globalization;

namespace HmPunyConverter
{
    class ToggleIDNIRI
    {
        public static bool ToggleIDNIRISupport(bool enable)
        {
            // 重要: IsWellFormedUriString() を一度初期化するために、ダミーで一度実行する必要がある。
            // static fields: s_IdnScope, s_IriParsing を初期化
            Uri.IsWellFormedUriString("http://example.com/query=ü", UriKind.Absolute); // この行は必要なので「なにこれゴミ？」とかでウッカリで消したりしないこと

            // クラスを含むアセンブリを取得
            Assembly assembly = Assembly.GetAssembly(typeof(Uri));
            if (assembly != null)
            {
                // クラスのタイプを得るために、アセンブリを直で使い、
                // .NET Frameworkで定義されているpublic/privateなどのアクセス修飾子のレベルを超越してガン無視してマニュアルで強制変更する。
                Type uriType = assembly.GetType("System.Uri");
                if (uriType != null)
                {
                    object idnScope = uriType.InvokeMember("s_IdnScope", BindingFlags.Static | BindingFlags.GetField | BindingFlags.NonPublic, null, null, new object[] { });
                    if (idnScope != null)
                    {
                        if (enable)
                        {
                            uriType.InvokeMember("s_IdnScope", BindingFlags.Static | BindingFlags.SetField | BindingFlags.NonPublic, null, null, new object[] { UriIdnScope.All });
                        }
                        else
                        {
                            uriType.InvokeMember("s_IdnScope", BindingFlags.Static | BindingFlags.SetField | BindingFlags.NonPublic, null, null, new object[] { UriIdnScope.None });
                        }
                    }

                    object iriParsing = uriType.InvokeMember("s_IriParsing", BindingFlags.Static | BindingFlags.GetField | BindingFlags.NonPublic, null, null, new object[] { });
                    if (iriParsing != null)
                    {
                        uriType.InvokeMember("s_IriParsing", BindingFlags.Static | BindingFlags.SetField | BindingFlags.NonPublic, null, null, new object[] { enable });
                    }
                }
            }

            return true;
        }

    }

    class UnicodeToPuny
    {
        static String convert(String url)
        {
            try
            {
                //UriオブジェクトからUriBuilderを作成する
                UriBuilder t = new UriBuilder(url);

                //Uriオブジェクトを作成する
                Uri u = new Uri(t.Uri.ToString());

                //UriオブジェクトからUriBuilderを作成する
                UriBuilder ub = new UriBuilder(u);

                // 日本語ドメインをpunnyへ
                ub.Host = u.IdnHost;

                // エスケープもしておく。
                var punny = ub.Uri.ToString();

                /*
                // もらったURLにそのスキームが含まれているならば
                if (url.StartsWith(ub.Scheme))
                {
                    // そのまま
                    return punny;
                }
                else
                {
                    // もともとスキームが含まれていなかったのであれば、スキーム分の文字列は除去
                    punny = punny.Replace(ub.Scheme + "://", "");
                    return punny;
                }
                */
                return punny;
            }
            catch (Exception)
            {

                // よくわからない場合
                IdnMapping idn = new IdnMapping();
                var punny = idn.GetAscii(url);
                return punny;
            }
        }

        public static String Convert(string url)
        {
            ToggleIDNIRI.ToggleIDNIRISupport(false);
            var ret = convert(url);
            return ret;
        }
    }

    class PunyToUnicode
    {
        static String _convert(String url)
        {
            try
            {
                //UriオブジェクトからUriBuilderを作成する
                UriBuilder ub = new UriBuilder(url);

                var unicode = ub.Uri.ToString();

                /*
                // もらったURLにそのスキームが含まれているならば
                if (url.StartsWith(ub.Scheme))
                {
                    // そのまま
                    return unicode;
                }
                else
                {
                    // もともとスキームが含まれていなかったのであれば、スキーム分の文字列は除去
                    unicode = unicode.Replace(ub.Scheme + "://", "");
                    return unicode;
                }
                */
                return unicode;
            }
            catch (Exception)
            {
                return url;
            }

        }

        public static String Convert(string url)
        {
            ToggleIDNIRI.ToggleIDNIRISupport(true);
            var ret = _convert(url);
            ToggleIDNIRI.ToggleIDNIRISupport(false);
            return ret;
        }
    }

    public class HmPunyConverter
    {
        const String kCmdPtoU = "-PtoU";
        const String kCmdUtoP = "-UtoP";

        static void Main(string[] args)
        {
            // 引数が無い
            if (args.Length == 0)
            {
                return;
            }

            String url = "";
            String cmd = "";

            // ちゃんとコマンドが指名してない時は自動
            if (args.Length >= 1)
            {
                url = args[0];
            }

            // punnyのようなら、日本語ドメインへ
            if (url.Contains("xn--"))
            {
                cmd = kCmdPtoU;
            }
            // それ以外ならpunnyへ
            else
            {
                cmd = kCmdUtoP;
            }

            // 変換結果
            String result = "";

            // 変換
            if (cmd == kCmdUtoP)
            {
                result = UnicodeToPuny.Convert(url);
            }
            else if (cmd == kCmdPtoU)
            {
                result = PunyToUnicode.Convert(url);
            }

            // 出力
            Console.WriteLine(result);

        }
    }
}
