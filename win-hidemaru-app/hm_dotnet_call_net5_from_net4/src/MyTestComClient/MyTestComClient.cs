using System;
using System.Runtime.InteropServices;


namespace MyTestCOMClient
{
    public class Program
    {
        public static string abc(string str)
        {
            try
            {
                var server = new MyTestComActivation.IMyTestComInterface();
                string[] list = { "aaa", "bbb", "ccc" };
                var add = server.AddString(str, "かきくけこ", list);
                return add;
            } catch(Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
            }
            return "";
        }
    }


    //次のクラスは通常PIAで定義されていますが、この例では
    //シナリオを単純化するためにここで定義されています。
    namespace MyTestComActivation
    {
        // CoClass のマネージドの定義
        [ComImport]
        [CoClass(typeof(MyTestComServer))]
        [Guid(ContractGuids.ServerInterface)] // TlbImp規則により、これを親インターフェイスのGUIDに設定します
        internal interface IMyTestComInterface : IMyTestCOMServer
        {
        }

        // CoClassのマネージドアクティベーション
        [ComImport]
        [Guid(ContractGuids.ServerClass)]
        internal class MyTestComServer
        {
        }
    }
}