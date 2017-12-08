/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Runtime.InteropServices;


// アンマネージドライブラリの遅延での読み込み。C++のLoadLibraryと同じことをするため
// これをする理由は、このhmRbとHideamru.exeが異なるディレクトリに存在する可能性があるため、
// C#風のDllImportは成立しないからだ。
internal sealed class UnManagedDll : IDisposable
{
    [DllImport("kernel32")]
    static extern IntPtr LoadLibrary(string lpFileName);
    [DllImport("kernel32")]
    static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);
    [DllImport("kernel32")]
    static extern bool FreeLibrary(IntPtr hModule);

    IntPtr moduleHandle;

    public UnManagedDll(string lpFileName)
    {
        moduleHandle = LoadLibrary(lpFileName);
    }

    public IntPtr ModuleHandle
    {
        get
        {
            return moduleHandle;
        }
    }

    public T GetProcDelegate<T>(string method) where T : class
    {
        IntPtr methodHandle = GetProcAddress(moduleHandle, method);
        T r = Marshal.GetDelegateForFunctionPointer(methodHandle, typeof(T)) as T;
        return r;
    }

    public void Dispose()
    {
        FreeLibrary(moduleHandle);
    }
}

