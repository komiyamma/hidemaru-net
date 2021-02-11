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

    public static implicit operator String(StaticWStrPtrHandle managedString)
    {
        return managedString.ToString();
    }

    [ReliabilityContract(Consistency.WillNotCorruptState, Cer.MayFail)]
    protected override bool ReleaseHandle()
    {
        try
        {
            Marshal.FreeHGlobal(handle);
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


