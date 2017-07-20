
using System;

using Hidemaru;

public class HmWordPopupDemo
{
    public static IntPtr Show()
    {
        try
        {
            if (HmWordPopupDemoForm.form != null)
            {
                HmWordPopupDemoForm.form.Close();
            }
            HmWordPopupDemoForm.form = new HmWordPopupDemoForm();
            HmWordPopupDemoForm.fontname = (String)Hm.Macro.Var["fontname"];
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
            if (HmWordPopupDemoForm.form != null)
            {
                HmWordPopupDemoForm.form.Close();
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }


}



