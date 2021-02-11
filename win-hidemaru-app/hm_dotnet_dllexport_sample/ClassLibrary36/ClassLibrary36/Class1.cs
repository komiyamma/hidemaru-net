using System;
using System.Runtime.InteropServices;

namespace ClassLibrary36
{

    public class Class1
    {
        static DllAssemblyResolver dasmr = new DllAssemblyResolver();
        // static DllNativeResolver dntvr;

        static Class1()
        {
            dasmr = new DllAssemblyResolver();
            // dntvr = new DllNativeResolver();
        }

        [DllExport]
        static int abc(int a, int b)
        {
//            IntPtr r = MyFunc((IntPtr)3);
            int c = (int)a + 3 + (int)r;
            return (int)c;
        }

        [DllImport("dll1")]
        static extern IntPtr MyFunc(IntPtr i);

        [return: MarshalAs(UnmanagedType.LPWStr)]
        [DllExport]
        static String efg([MarshalAs(UnmanagedType.LPWStr)] String a, [MarshalAs(UnmanagedType.LPWStr)] String b)
        {
            return a + b;
        }
    }
}
