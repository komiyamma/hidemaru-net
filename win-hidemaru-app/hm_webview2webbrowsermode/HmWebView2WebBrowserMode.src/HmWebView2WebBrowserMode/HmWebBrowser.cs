/*
 * Copyright (c) 2021 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using Hidemaru;
using System;
using System.Text;

public class HmWebView2WebBrowserMode
{
    private static bool HmWebBrowserModeFormClosed()
    {
        return HmWebView2WebBrowserModeForm.form == null || HmWebView2WebBrowserModeForm.form.IsClosed();
    }

    public static IntPtr Create()
    {
        try
        {
            HmWebView2WebBrowserModeForm.SetContinueNotify();
            if (HmWebBrowserModeFormClosed())
            {
                String fontname = (String)Hm.Macro.Var["fontname"];
                /*
                Int32 argb = (Int32)(dynamic)Hm.Macro.Var["tcolor"];
                Color tcolor = Color.FromArgb(argb);
                */
                HmWebView2WebBrowserModeForm.form = new HmWebView2WebBrowserModeForm(fontname);
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
            return HmWebView2WebBrowserModeForm.form.GetWebBrowserDocumentText();
        }
        return "";
    }

    public static IntPtr Destroy()
    {
        try
        {
            if (HmWebView2WebBrowserModeForm.form != null)
            {
                HmWebView2WebBrowserModeForm.form.Close();
                HmWebView2WebBrowserModeForm.form = null;
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



