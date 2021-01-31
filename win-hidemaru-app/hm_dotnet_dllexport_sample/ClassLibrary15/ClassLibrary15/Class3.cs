/***
using System;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Security.Permissions;


[SecurityPermission(SecurityAction.Demand, UnmanagedCode = true)]
internal sealed class StaticWStrPtrHandle : SafeHandle
{
    private StaticWStrPtrHandle() : base(IntPtr.Zero, true) { }
    public StaticWStrPtrHandle(String managedString) : base(IntPtr.Zero, true)
    {
        handle = Marshal.StringToHGlobalUni(managedString);
    }

    [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
    protected override bool ReleaseHandle()
    {
        try
        {
            Marshal.FreeHGlobal(handle);
            System.Console.WriteLine("ReleaseHandle");
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine("文字列解放エラー:\n" + ex.Message);
            return false;
        }

        return true;
    }

    public override bool IsInvalid
    {
        get { return (IntPtr.Zero == handle); }
    }

    public override string ToString()
    {
        if (this.IsInvalid)
        {
            return String.Empty;
        }
        else
        {
            return Marshal.PtrToStringUni(handle);
        }
    }

}


namespace ConsoleApp31
{
    static class Program
    {
        static String rtnBufferOfGetString = "abc";
        static unsafe char* abc()
        {
            rtnBufferOfGetString = "eee";
            fixed (char* p_str = rtnBufferOfGetString) { return p_str; }
        }

        static unsafe void ddd()
        {
            System.Console.WriteLine("1");
            char* b = abc();
            GC.Collect();
            GC.Collect();
            GC.Collect();
            GC.Collect();
            GC.Collect();
            String c = new string(b);
            System.Console.WriteLine(c);
        }

        static void Main(string[] args)
        {

            ddd();

        }
    }
}
*/