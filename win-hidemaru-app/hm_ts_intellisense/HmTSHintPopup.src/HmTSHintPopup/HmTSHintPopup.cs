
using System;

using Hidemaru;

public class HmTSHintPopup
{
    public static IntPtr Start()
    {
        try
        {
            if (HmTSHintPopupForm.form == null)
            {
                String fontname = (String)Hm.Macro.Var["fontname"];
                HmTSHintPopupForm.form = new HmTSHintPopupForm(fontname);
            }
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
            if (HmTSHintPopupForm.form != null)
            {
                HmTSHintPopupForm.form.Close();
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }


}



