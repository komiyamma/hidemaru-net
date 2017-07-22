
using System;

using Hidemaru;

public class HmTSDefineJump
{
    public static IntPtr Start()
    {
        try
        {
            HmTSDefineJumpForm.StartTSServer();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }

    public static IntPtr TagJump()
    {
        try
        {
            HmTSDefineJumpForm.StartTSServer();
            HmTSDefineJumpForm.TagJump();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            HmTSDefineJumpForm.StopTSServer();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }


}



