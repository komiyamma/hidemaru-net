using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace MyTestCOMServer
{
    [ComVisible(true)]
    [Guid(ContractGuids.ServerClass)]
    public class Server : IMyTestCOMServer
    {
        string IMyTestCOMServer.AddString(string str1, string str2)
        {
            return str1 + str2;
        }
    }
}