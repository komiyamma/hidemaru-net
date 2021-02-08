using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;



internal class DllNativeResolver
{
    [DllImport("kernel32")]
    private static extern IntPtr LoadLibrary(string lpFileName);


    const string dllName = "Dll1.dll";

    public DllNativeResolver()
    {
        var selfdir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

        // Environment.Is64BitProcess ? "x64" : "x86" ← 解説時にはこの要素を使うことで振り分けることも多いことも盛り込む
        var subDir = Environment.Is64BitProcess ? "x64" : "x86";
        var dllPath = Path.Combine(selfdir, subDir, dllName);
        System.Diagnostics.Trace.WriteLine(dllPath);
        // var dllPath = Path.Combine(selfdir, dllName);

        if (!(File.Exists(dllPath) && LoadLibrary(dllPath) != IntPtr.Zero))
        {
            System.Diagnostics.Trace.WriteLine(dllName + " の読み込みに失敗しました");
            throw new DllNotFoundException(dllPath);
        }
    }
}


/*
internal class DllNativeResolver
{
    [DllImport("kernel32", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr AddDllDirectory(string NewDirectory);
    static IntPtr valudOfDllDirectoryCoolie = IntPtr.Zero;

    [DllImport("kernel32", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern int RemoveDllDirectory(IntPtr valudOfDllDirectoryCoolie);

    public DllNativeResolver()
    {
        var selfdir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        valudOfDllDirectoryCoolie = AddDllDirectory(selfdir);
    }

    ~DllNativeResolver()
    {
        // このようなRemoveしてしまう記述は、一見行儀が良いように思えるが、実際には望ましくはないだろう。
        if (valudOfDllDirectoryCoolie != IntPtr.Zero)
        {
            RemoveDllDirectory(valudOfDllDirectoryCoolie);
        }
    }
}
*/