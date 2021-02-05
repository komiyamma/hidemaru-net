/*
 * Copyright (c) 2021 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using Hidemaru;
using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

internal class NativeMethods {
    [DllImport("kernel32")]
    private static extern IntPtr LoadLibrary(string lpFileName);

    const string dllName = "WebView2Loader.dll";

    static NativeMethods()
    {
        var dllPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), dllName);
        if (!(File.Exists(dllPath) && LoadLibrary(dllPath) != IntPtr.Zero)) {
            System.Diagnostics.Trace.WriteLine(dllName + " の読み込みに失敗しました");
            throw new DllNotFoundException(dllPath);
        }
    }
}

public class HmWebView2BrowserMode
{
    static NativeMethods nm;
    static HmWebView2BrowserMode()
    {
        nm = new NativeMethods();
    }
    private static bool HmWebBrowserModeFormClosed()
    {
        return HmWebView2BrowserModeForm.form == null || HmWebView2BrowserModeForm.form.IsClosed();
    }

    public static IntPtr Create()
    {
        try
        {
            HmWebView2BrowserModeForm.SetContinueNotify();
            if (HmWebBrowserModeFormClosed())
            {
                String fontname = (String)Hm.Macro.Var["fontname"];
                /*
                Int32 argb = (Int32)(dynamic)Hm.Macro.Var["tcolor"];
                Color tcolor = Color.FromArgb(argb);
                */
                HmWebView2BrowserModeForm.form = new HmWebView2BrowserModeForm(fontname);
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
            return HmWebView2BrowserModeForm.form.GetWebBrowserDocumentText();
        }
        return "";
    }

    public static IntPtr Destroy()
    {
        try
        {
            if (HmWebView2BrowserModeForm.form != null)
            {
                HmWebView2BrowserModeForm.form.Close();
                HmWebView2BrowserModeForm.form = null;
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



