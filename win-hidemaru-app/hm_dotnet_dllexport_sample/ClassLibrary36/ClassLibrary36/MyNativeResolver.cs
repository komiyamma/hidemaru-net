using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace ClassLibrary36
{

    internal class NativeMethods
    {
        [DllImport("kernel32")]
        private static extern IntPtr LoadLibrary(string lpFileName);

        const string dllName = "WebView2Loader.dll";

        NativeMethods()
        {
            // Environment.Is64BitProcess ? "x64" : "x86" ← 解説時にはこの要素を使うことで振り分けることも多いことも盛り込む
            var dllPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), dllName);

            if (!(File.Exists(dllPath) && LoadLibrary(dllPath) != IntPtr.Zero))
            {
                System.Diagnostics.Trace.WriteLine(dllName + " の読み込みに失敗しました");
                throw new DllNotFoundException(dllPath);
            }
        }
    }

}
