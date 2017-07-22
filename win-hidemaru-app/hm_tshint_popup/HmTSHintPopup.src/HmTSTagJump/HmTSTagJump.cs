
using System;

using Hidemaru;

public class HmTSTagJump
{
    public static IntPtr Start()
    {
        try
        {
            HmTSTagJumpForm.StartTSServer();
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
            HmTSTagJumpForm.StartTSServer();
            HmTSTagJumpForm.TagJump();
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
            HmTSTagJumpForm.StopTSServer();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }


}



