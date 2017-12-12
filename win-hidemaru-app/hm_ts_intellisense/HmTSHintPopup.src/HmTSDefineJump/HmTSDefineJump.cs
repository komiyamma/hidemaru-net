/* 
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;

using Hidemaru;

public class HmTSDefineJump
{
    public static IntPtr Start()
    {
        try
        {
            HmTSDefineJumpForm.StartTSServer();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }

    public static IntPtr DefineJump()
    {
        try
        {
            HmTSDefineJumpForm.StartTSServer();
            HmTSDefineJumpForm.CommandDelegate dlgQueryCommandd = HmTSDefineJumpForm.DefinitionMessage;
            HmTSDefineJumpForm.QueryCommand(dlgQueryCommandd);
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }

    public static IntPtr ReferencesList()
    {
        try
        {
            HmTSDefineJumpForm.StartTSServer();
            HmTSDefineJumpForm.CommandDelegate dlgQueryCommandd = HmTSDefineJumpForm.ReferencesMessage;
            HmTSDefineJumpForm.QueryCommand(dlgQueryCommandd);
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
            HmTSDefineJumpForm.StopTSServer();
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }

        return (IntPtr)1;
    }


}



