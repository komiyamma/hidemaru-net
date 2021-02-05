using System;
using System.Runtime.InteropServices;
using HtmlAgilityPack;

namespace ClassLibrary36 {

    public class Class1
    {
        static DllAssemblyResolver dasmr;
        static Class1() {

            dasmr = new DllAssemblyResolver();
            System.Diagnostics.Trace.WriteLine("MyAssemblyResolver:");
        }

        [DllExport]
        static int abc(int a)
        {
            int b = (int)a + 3;
            return (int)b;
        }

        [return: MarshalAs(UnmanagedType.LPWStr)]
        [DllExport]
        static String efg([MarshalAs(UnmanagedType.LPWStr)]String a, [MarshalAs(UnmanagedType.LPWStr)] String b)
        {
            try
            {
                // From String
                var doc = new HtmlDocument();
                doc.LoadHtml("<html></html>");
                System.Diagnostics.Trace.WriteLine(doc.ToString());
            } catch(Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e);

            }
            return a +b;
        }
    }
}
