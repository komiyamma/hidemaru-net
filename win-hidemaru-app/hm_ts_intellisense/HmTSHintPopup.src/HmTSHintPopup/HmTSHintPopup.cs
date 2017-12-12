/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;

using Hidemaru;

public class HmTSHintPopup
{
    private static bool HmTSHintPopupFormClosed()
    {
        return HmTSHintPopupForm.form == null || HmTSHintPopupForm.form.IsClosed();
    }

    public static IntPtr IsNull()
    {
        if (HmTSHintPopupFormClosed())
        {
            return (IntPtr)1;
        }
        else
        {
            return (IntPtr)0;
        }
    }

    public static IntPtr Start()
    {
        try
        {
            HmTSHintPopupForm.SetContinueNotify();
            if (HmTSHintPopupFormClosed())
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

    public static IntPtr Stop()
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

    public static IntPtr OnDetachMethod()
    {
        Stop();

        return (IntPtr)1;
    }


}



