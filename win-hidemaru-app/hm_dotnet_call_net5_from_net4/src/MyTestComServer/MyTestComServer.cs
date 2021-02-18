using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace MyTestCOMServer
{
    [ComVisible(true)]
    [Guid(ContractGuids.ServerClass)]
    public class MyTestCOMServer : IMyTestCOMServer
    {

        string IMyTestCOMServer.AddString(string str1, string str2, string[] list)
        {
            return str1 + str2 + list.Length.ToString(); ;
        }
    }
}