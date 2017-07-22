
using System;

using Hidemaru;

public class HmTSTagJump
{
    public static IntPtr TagJump()
    {
        try
        {
            if (HmTSTagJumpForm.form != null)
            {
                HmTSTagJumpForm.form.Close();
            }
            HmTSTagJumpForm.form = new HmTSTagJumpForm();
            HmTSTagJumpForm.form.TagJump();
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
            if (HmTSTagJumpForm.form != null)
            {
                HmTSTagJumpForm.form.Close();
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }


}



