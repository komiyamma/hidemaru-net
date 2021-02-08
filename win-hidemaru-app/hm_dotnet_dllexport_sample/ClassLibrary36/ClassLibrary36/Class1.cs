using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Microsoft.Web.WebView2.WinForms;

namespace ClassLibrary36
{

    public class Class1
    {
        static DllAssemblyResolver dasmr = new DllAssemblyResolver();
        static DllNativeResolver dntvr;

        static Class1()
        {
            dntvr = new DllNativeResolver();
        }

        class MyForm : Form
        {
            WebView2 wb = new WebView2
            {
                Source = new Uri("https://www.google.com")
            };

            public MyForm()
            {
                wb.Size = this.Size;
                this.Controls.Add(wb);
            }
        }

        [DllExport]
        static int abc(int a)
        {
            IntPtr r = MyFunc((IntPtr)3);
            int b = (int)a + 3 + (int)r;
            return (int)b;
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
