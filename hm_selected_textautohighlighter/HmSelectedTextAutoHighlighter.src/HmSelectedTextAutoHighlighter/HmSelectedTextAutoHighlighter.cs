/*
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;


public class HmSelectedTextAutoHighlighter
{
    public static IntPtr Start()
    {
        try
        {
            if (HmSelectedTextAutoHighlighterForm.form == null)
            {
                HmSelectedTextAutoHighlighterForm.form = new HmSelectedTextAutoHighlighterForm();
            }
            return (IntPtr)1;
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex);
            return (IntPtr)0;
        }
    }

    public static IntPtr OnDetachMethod()
    {
        try
        {
            if (HmSelectedTextAutoHighlighterForm.form != null)
            {
                HmSelectedTextAutoHighlighterForm.form.Stop();
                HmSelectedTextAutoHighlighterForm.form = null;
            }
            return (IntPtr)1;
        }
        catch (Exception ex)
        {
            System.Diagnostics.Trace.WriteLine(ex);
            return (IntPtr)0;
        }
    }
}
