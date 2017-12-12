/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using System;
using System.Drawing;

using Hidemaru;
using System.Text;

public class HmWebBrowserMode
{
    private static bool HmWebBrowserModeFormClosed()
    {
        return HmWebBrowserModeForm.form == null || HmWebBrowserModeForm.form.IsClosed();
    }

    public static IntPtr Create()
    {
        try
        {
            HmWebBrowserModeForm.SetContinueNotify();
            if (HmWebBrowserModeFormClosed())
            {
                String fontname = (String)Hm.Macro.Var["fontname"];
                /*
                Int32 argb = (Int32)(dynamic)Hm.Macro.Var["tcolor"];
                Color tcolor = Color.FromArgb(argb);
                */
                HmWebBrowserModeForm.form = new HmWebBrowserModeForm(fontname);
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
            System.Diagnostics.Trace.WriteLine(e.StackTrace);
        }

        return (IntPtr)1;
    }

    public static IntPtr IsNull()
    {
        if (HmWebBrowserModeFormClosed())
        {
            return (IntPtr)1;
        }
        else
        {
            return (IntPtr)0;
        }
    }

    public static String GetDocumentText()
    {
        if (!HmWebBrowserModeFormClosed())
        {
            return HmWebBrowserModeForm.form.GetWebBrowserDocumentText();
        }
        return "";
    }

    public static String GetDocumentTextAsEncoding(IntPtr encoding)
    {
        if (!HmWebBrowserModeFormClosed())
        {
            var enc = Encoding.UTF8;
            try
            {
                enc = Encoding.GetEncoding(encoding.ToInt32());
            } catch (Exception e)
            {
                System.Diagnostics.Trace.WriteLine(e.Message);
            }
            return HmWebBrowserModeForm.form.GetWebBrowserDocumentTextAsEncoding(enc);
        }
        return "";
    }

    public static IntPtr Destroy()
    {
        try
        {
            if (HmWebBrowserModeForm.form != null)
            {
                HmWebBrowserModeForm.form.Close();
                HmWebBrowserModeForm.form = null;
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
        Destroy();

        return (IntPtr)1;
    }


}



