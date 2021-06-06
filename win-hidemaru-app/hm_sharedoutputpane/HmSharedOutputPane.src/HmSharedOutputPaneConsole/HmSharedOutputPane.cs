using System;
using System.Runtime.InteropServices;
 
 
class Program
{
    [DllImport("HmSharedOutputPane.dll", CharSet=CharSet.Unicode)]
    private extern static void SetSharedMessageW(String msg);
 
    static void Main(string[] args)
    {
        if (args.Length > 0) { 
            SetSharedMessageW(args[0]);
        }
    }
}