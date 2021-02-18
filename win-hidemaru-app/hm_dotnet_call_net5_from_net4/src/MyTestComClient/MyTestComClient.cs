using System;
using System.Runtime.InteropServices;


namespace MyTestCOMClient
{
    public class Program
    {
        public static string abc(string args)
        {
            try
            {
                var server = new MyTestComActivation.IMyTestComInterface();
                string[] list = { "aaa", "bbb", "ccc" };
                var add = server.AddString("あいうえお", "かきくけこ", list);
                return add;
            } catch(Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
            }
            return "";
        }

        public static void Main(string[] args)
        {
            try
            {
                string ret = abc("dummy");
                Console.WriteLine(ret);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }


    //次のクラスは通常PIAで定義されていますが、この例では
    //シナリオを単純化するためにここで定義されています。
    namespace MyTestComActivation
    {
        /// <summary>
        /// CoClass のマネージドの定義
        /// </summary>
        [ComImport]
        [CoClass(typeof(MyTestComServer))]
        [Guid(ContractGuids.ServerInterface)] // TlbImp規則により、これを親インターフェイスのGUIDに設定します
        internal interface IMyTestComInterface : IMyTestCOMServer
        {
        }

        /// <summary>
        /// CoClassのマネージドアクティベーション
        /// </summary>
        [ComImport]
        [Guid(ContractGuids.ServerClass)]
        internal class MyTestComServer
        {
        }
    }
}