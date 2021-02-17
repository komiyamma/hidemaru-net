using System;
using System.Runtime.InteropServices;




namespace COMClient
{
    public class Program
    {
        public static string abc(string args)
        {
            try
            {
                var server = new MyComActivation.MyTestComInterface();

                var add = server.AddString("あいうえお", "かきくけこ");
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
                var server = new MyComActivation.MyTestComInterface();

                var add = server.AddString("あいうえお", "かきくけこ");
                Console.WriteLine(add);
                
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }
        }
    }


    //次のクラスは通常PIAで定義されていますが、この例では
    //シナリオを単純化するためにここで定義されています。
    namespace MyComActivation
    {
        /// <summary>
        /// CoClass のマネージドの定義
        /// </summary>
        [ComImport]
        [CoClass(typeof(MyTestComServer))]
        [Guid(ContractGuids.ServerInterface)] // TlbImp規則により、これを親インターフェイスのGUIDに設定します
        internal interface MyTestComInterface : IMyTestCOMServer
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