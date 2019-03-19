using System;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using Hidemaru;

namespace HmGoogleLangTranslate
{

    // POST をアップでする際の、jsonの形
    [DataContract]
    internal class PostUploadData
    {
        [DataMember(Name = "text")]
        public string Text { get; private set; }

        [DataMember(Name = "source")]
        public string Source { get; private set; }

        [DataMember(Name = "target")]
        public string Target { get; private set; }

        public PostUploadData(string text, string source, string target)
        {
            this.Text = text;
            this.Source = source;
            this.Target = target;
        }
    }

    // POST で返ってくる際の、jsonの形
    [DataContract]
    internal class PostDownloadData
    {
        [DataMember(Name = "result")]
        public string Result { get; set; }
    }

    internal class HmGoogleLangTranslateHelper
    {
        public static async Task<string> ResuestAndRecieve(string text, string url, string lang_src, string lang_dst)
        {
            // C#のJsonオブジェクトをJson文字列に
            string json = JsonUtilityExtension.ToJson<PostUploadData>(new PostUploadData(text, lang_src, lang_dst));
            using (var client = new HttpClient())
            {
                // httpの基本情報設定
                client.DefaultRequestHeaders.Accept.Clear();
                var accept = new MediaTypeWithQualityHeaderValue("application/json");
                client.DefaultRequestHeaders.Accept.Add(accept);

                // json文字列をネットリクエストの内容として設定
                System.Net.Http.StringContent content = new System.Net.Http.StringContent(json, Encoding.UTF8, "application/json");

                // Postで内容を投げる（非同期なんでPostが終わるのを待つ)
                HttpResponseMessage res = await client.PostAsync(url, content);

                // Postの結果を読み取り(非同期なんで読み取り終わるのを待つ)
                string result = await res.Content.ReadAsStringAsync();

                // 
                return result;
            }
        }
    }
}



namespace HmGoogleLangTranslate
{
    public partial class HmGoogleLangTranslate
    {

        public static string Translate(string text, string url, string lang_src, string lang_dst)
        {
            try
            {
                // 少し面倒だが、ヘルパーメソッドがAsyncで構成されてるので、Task生成で、Waitして、結果を返す。
                Task<string> t = TranslateAsync(text, url, lang_src, lang_dst);
                t.Wait();
                return t.Result;
            }
            catch {; }

            return "";
        }

        private static async Task<string> TranslateAsync(string text, string url, string lang_src, string lang_dst)
        {
            // リクエストに投げて結果待ち
            string result_json = await HmGoogleLangTranslateHelper.ResuestAndRecieve(text, url, lang_src, lang_dst);

            // 結果は文字列のJsonなので、C#のオブジェクトに変換
            PostDownloadData result_data_obj = JsonUtilityExtension.FromJson(new PostDownloadData(), result_json);

            // Resultプロパティが翻訳された内容のテキスト
            return result_data_obj.Result;
        }
    }
}
