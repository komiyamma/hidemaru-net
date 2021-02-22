using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace NET4COMServer
{
    [Guid("BD55F2A6-9ED0-4F4F-9D37-E6B84BE63272")]
    public class NET4COMServer
    {
        public string str_add(string a, string b)
        {
            return a + b;
        }

        public int int_add(int a, int b)
        {
            return a + b;
        }
    }
}
