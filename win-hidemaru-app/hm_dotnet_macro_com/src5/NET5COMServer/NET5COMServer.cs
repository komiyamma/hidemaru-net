using System;
using System.Runtime.InteropServices;

namespace NET5COMServer
{

    [ComVisible(true)]
    [Guid("C144B4C1-27CB-401F-ABE8-55378564E18D")]
    public class NET5COMServer
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
