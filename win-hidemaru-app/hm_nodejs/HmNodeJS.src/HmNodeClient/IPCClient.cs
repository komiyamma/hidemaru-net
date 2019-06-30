/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Runtime.Remoting.Channels.Ipc;
using System.Security.Permissions;
using System.Collections.Generic;

class HmNodeIPCClient
{
    // キーがIP、値はResponseData型で、それぞれのチャンネルでのレスポンスデータを保持しておく。
    // 後で、リクエストしなおすことなく、GetLastResponseなどを可能とするため。
    static Dictionary<int, IpcChannel> ch = new Dictionary<int, IpcChannel>();

    [SecurityPermission(SecurityAction.Demand)]
    protected static bool RegisterChannnel(int IP)
    {
        // リモート用として、クライアントチャンネルを登録。登録系は同じプロセス内で複数回やるとエラーが出るので、
        try
        {
            System.Runtime.Remoting.Channels.ChannelServices.RegisterChannel(ch[IP], false);

            // リモート呼び出しのため、(サーバーにある)オブジェクト取得。
            // Register as client for remote object.
            System.Runtime.Remoting.WellKnownClientTypeEntry remoteType = new System.Runtime.Remoting.WellKnownClientTypeEntry(typeof(HmNodeRemoteObject), "ipc://localhost:" + IP + "/" + HmNodeRemoteObject.Name + IP + ".rem");

            // クライアントタイプとして登録
            System.Runtime.Remoting.RemotingConfiguration.RegisterWellKnownClientType(remoteType);

            // 通知先(Message Sink)のインスタンス生成
            string objectUri;
            System.Runtime.Remoting.Messaging.IMessageSink messageSink = (ch[IP]).CreateMessageSink("ipc://localhost:" + IP + "/" + HmNodeRemoteObject.Name + IP + ".rem", null, out objectUri);
            Console.WriteLine("通知先のURIは {0}.", objectUri);
            if (messageSink != null)
            {
                Console.WriteLine("通知先のクラス型は {0}.", messageSink.GetType().ToString());
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Debug.WriteLine(e.GetType().ToString());
            System.Diagnostics.Debug.WriteLine(e.Message);

            return false;
        }

        return true;
    }

    public static void UnregisterChannnel(int IP)
    {
        // ハッシュにキーがなければ登録
        if (!ch.ContainsKey(IP))
        {
            return;
        }
        // リモート用として、クライアントチャンネルを登録。登録系は同じプロセス内で複数回やるとエラーが出るので、
        try
        {
            System.Runtime.Remoting.Channels.ChannelServices.UnregisterChannel(ch[IP]);

        }
        catch (Exception e)
        {
            Console.WriteLine(e.Message);
        }
    }


    [SecurityPermission(SecurityAction.Demand)]
    public static HmNodeRemoteObject.ResponseData GetResponse(int IP, System.Collections.Specialized.NameValueCollection param, bool need_request = true)
    {
        // ハッシュにキーがなければ登録
        if (!ch.ContainsKey(IP))
        {
            ch[IP] = new IpcChannel();
        }

        try
        {
            RegisterChannnel(IP);

            // リモートオブジェクトのインスタンスの生成.
            HmNodeRemoteObject service = new HmNodeRemoteObject();

            // リモートオブジェクトのメソッドの実行
            Console.WriteLine("クライアントからリモートオブジェクトを実行。");
            HmNodeRemoteObject.ResponseData res;
            if (need_request)
            {
                res = service.GetResponse(IP, param);
                Console.WriteLine("リモートオブジェクトは {0} 回呼ばれました。", res.CallCount);
            }
            else
            {
                res = service.GetLastResponse(IP);
            }
            Console.WriteLine("{0}", res.Data);
            Console.WriteLine("{0}", res.StatusCode);
            Console.WriteLine("{0}", res.StatusDescription);

            return res;
        }
        catch (Exception e)
        {
            HmNodeRemoteObject.ResponseData res = new HmNodeRemoteObject.ResponseData();
            res.Data = "";
            res.StatusCode = e.GetType().ToString();
            res.StatusDescription = e.Message;
            return res;
        }
    }

    public static HmNodeRemoteObject.ResponseData GetLastResponse(int IP)
    {
        return GetResponse(IP, new System.Collections.Specialized.NameValueCollection(), false);
    }

}