
using System;

using Hidemaru;

public class HmWebBrowserMode
{
    public static IntPtr Create()
    {
        _NotifyWnd(Hm.WindowHandle);
        try
        {
            if (HmWebBrowserModeForm.form == null)
            {
                String fontname = (String)Hm.Macro.Var["fontname"];
                HmWebBrowserModeForm.form = new HmWebBrowserModeForm(fontname);
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }

    public static IntPtr IsNull()
    {
        if (HmWebBrowserModeForm.form == null)
        {
            return (IntPtr)1;
        }
        else
        {
            return (IntPtr)0;
        }
    }

    public static IntPtr Notify()
    {
        _NotifyWnd(Hm.WindowHandle);

        return (IntPtr)1;
    }
    private static IntPtr _NotifyWnd(IntPtr hCurrentActiveWndHidemaru)
    {
        HmWebBrowserModeForm.WriteSharedMemory(hCurrentActiveWndHidemaru);

        return (IntPtr)0;
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
            _NotifyWnd(IntPtr.Zero);
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



