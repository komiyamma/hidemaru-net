using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Net;

// リモートオブジェクトのクラス。MarshalByRefObject から継承することで、
// IPCチャンネルなどのサーバー・クライアント共通のリモートオブジェクトクラスとなりえる。
public class HmNodeRemoteObject : MarshalByRefObject
{
    // リモートオブジェクトのファイル名。この名前だけではなく、IP名も別途足している。
    public const string Name = "HmNodeRemoteObject";

    // シリアライズ可。外部プログラムはこの構造体のフィールド名を覗いてくる。
    [Serializable]
    public struct ResponseData
    {
        public int CallCount;               // 呼ばれた回数
        public string Data;                 // リクエストして得たデータ
        public string StatusCode;           // エラーだった時のエラークラス
        public string StatusDescription;    // 
    }

    // キーがIP、値はResponseData型で、それぞれのチャンネルでのレスポンスデータを保持しておく。
    // 後で、リクエストしなおすことなく、GetLastResponseなどを可能とするため。
    static Dictionary<int, ResponseData> ht = new Dictionary<int, ResponseData>();

    // ノードにクエリーを出す
    public ResponseData GetResponse(int IP, NameValueCollection query)
    {
        // ハッシュにキーがなければ登録
        if (!ht.ContainsKey(IP))
        {
            ht[IP] = new ResponseData();
        }
        ResponseData data = ht[IP];
        data.CallCount++; // 呼び出して継続しているかどうか確認のため

        // ノードにアクセスしてみる。GETアクセスのみ。POSTアクセスだとNode側が扱いにくいだろうから、
        try
        {
            string url = "http://localhost:" + IP + "/";

            System.Net.WebClient wc = new System.Net.WebClient();
            //NameValueCollectionの作成
            System.Collections.Specialized.NameValueCollection ps;
            if (query == null)
            {
                ps = new NameValueCollection();
            }
            else
            {
                ps = query;
            }

            string resText = "";

            if (true)
            {
                wc.QueryString = ps;
                //データを送信し、また受信する
                byte[] resData = wc.DownloadData(url);
                resText = System.Text.Encoding.UTF8.GetString(resData);
            }
            else
            {
                //データを送信し、また受信する
                byte[] resData = wc.UploadValues(url, ps);
                resText = System.Text.Encoding.UTF8.GetString(resData);
            }

            // 取得したデータを ResponseData 型のプロパティに
            data.Data = resText;
            data.StatusCode = "OK";
            data.StatusDescription = "OK";

            wc.Dispose();

            ht[IP] = data;
            return data;
        }
        // エラーが起きたら、ほとんどの場合はこれ。StatusCodeや、Resposeがある。
        catch (WebException e)
        {
            data.Data = "";
            data.StatusCode = e.Status.ToString();
            data.StatusDescription = e.Response.ToString() + "\n" + e.Message;
            ht[IP] = data;

            return data;
        }
        // かなり珍しいだろうけれど、それ以外のエラーの場合
        catch (Exception e)
        {
            data.Data = "";
            data.StatusCode = e.GetType().ToString();
            data.StatusDescription = e.Message;
            ht[IP] = data;

            return data;
        }
    }

    // 最後のデータをそのまま取得
    public ResponseData GetLastResponse(int IP)
    {
        if (!ht.ContainsKey(IP))
        {
            ht[IP] = new ResponseData();
        }
        return ht[IP];
    }


    /// 時間で放置しておくと、自動的に切断されるのを回避
    public override object InitializeLifetimeService()
    {
        return null;
    }
}
