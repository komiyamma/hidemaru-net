﻿/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Collections.ObjectModel;


// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
public class IEdgeJSStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmEdgeJSDynamicLib.OutputDebugStream(error);
    }

    public static IntPtr CreateScope()
    {
        return hmEdgeJSDynamicLib.CreateScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmEdgeJSDynamicLib.BindDllHandle(dll);
    }

    public static void SetCodePage(IntPtr cp)
    {
        hmEdgeJSDynamicLib.SetCodePage(cp);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmEdgeJSDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmEdgeJSDynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------
    public static IntPtr GetNumVar(String mng_var_name)
    {
        return hmEdgeJSDynamicLib.GetNumVar(mng_var_name);
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        return hmEdgeJSDynamicLib.SetNumVar(mng_var_name, value);
    }

    public static String GetStrVar(String mng_var_name)
    {
        return hmEdgeJSDynamicLib.GetStrVar(mng_var_name);
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        return hmEdgeJSDynamicLib.SetStrVar(mng_var_name, value);
    }
    //-----------------------------------------------------------------


    public static IntPtr DoString(string expression)
    {
        return hmEdgeJSDynamicLib.DoString(expression);
    }

    public static IntPtr DoFile(string filename)
    {
        return hmEdgeJSDynamicLib.DoFile(filename);
    }

    public static IntPtr DestroyScope()
    {
        return hmEdgeJSDynamicLib.DestroyScope();
    }

}




// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmEdgeJSDynamicLib
{
    static hmEdgeJSDynamicLib()
    {
        try
        {
            StringBuilder exe_full_path = new StringBuilder(260);
            GetModuleFileName(IntPtr.Zero, exe_full_path, (ulong)exe_full_path.Capacity);
            strExecuteFullpath = exe_full_path.ToString();
        } catch(Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

    static String strExecuteFullpath;
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern ulong GetModuleFileName(IntPtr hModule, StringBuilder filename, ulong bufsize);

    // public static Runspace runspace;
    // public static Pipeline pipeline;

    public static void OutputDebugStream(String error)
    {
        System.Diagnostics.Trace.WriteLine(error);
    }

    // hmのため
    //----------------------------------------------------------------------------------------------
    public static Hidemaru hm;

    static readonly String strDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;

    // loadllした時の、変数の値。(そのマクロ内で何番めにloaddllしたかで値が変わる)
    static int iDllBindHandle = 0; // 1以上の整数。hmでloaddllした時の値
    public static IntPtr BindDllHandle(IntPtr dll)
    {
        iDllBindHandle = dll.ToInt32();
        return dll;
    }

    // dllのloaddllタイプによって、渡されたcmd(=expression)に対して、「dllの番号,」or「」を当てはめる処理
    public static String ModifyFuncCallByDllType(String cmd)
    {
        if (iDllBindHandle == -1)
        {
            return String.Format(cmd, "");
        }
        else
        {
            return String.Format(cmd, iDllBindHandle.ToString() + ", ");
        }
    }

    private static Object tmpVar = null;
    public static IntPtr SetTmpVar(Object value)
    {
        tmpVar = value;
        return (IntPtr)1;
    }

    public static Object PopTmpVar()
    {
        return tmpVar;
    }


    //----------------------------------------------------------------------------------------------
    public static IntPtr CreateScope()
    {
        /*
        // まだエンジンが作られていなければ
        if (runspace == null)
        {
            try
            {
                // create Powershell runspace
                runspace = RunspaceFactory.CreateRunspace();

                // open it
                runspace.Open();


                hm = new Hidemaru();
                runspace.SessionStateProxy.SetVariable("hm", hm);
                runspace.SessionStateProxy.SetVariable("AssemblyPath", new List<String>());
                dpr = new DllPathResolver();

                // ダミー関数の定義
                DoString("function DestroyScope() {}");

                return (IntPtr)1;
            }
            catch (Exception e)
            {
                OutputDebugStream(e.Message + "生成時エラー");
                return (IntPtr)0;
            }

        }
        */
        return (IntPtr)1;
    }

    const int default_codepage = 65001;
    static int codepage = default_codepage;
    public static void SetCodePage(IntPtr cp)
    {
        codepage = cp.ToInt32();
    }

    public static IntPtr GetNumVar(String mng_var_name)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        /*
        try
        {
            Object var = runspace.SessionStateProxy.GetVariable(mng_var_name);
            if (IntPtr.Size == 4)
            {
                return (IntPtr)(Convert.ToInt32(var));
            }
            else
            {
                return (IntPtr)(Convert.ToInt64(var));
            }
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        */
        return (IntPtr)0;
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {

        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        /*
        try
        {
            if (IntPtr.Size == 4)
            {
                runspace.SessionStateProxy.SetVariable(mng_var_name, value.ToInt32());
            }
            else
            {
                runspace.SessionStateProxy.SetVariable(mng_var_name, value.ToInt64());
            }
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        */
        return (IntPtr)0;
    }

    public static String GetStrVar(String mng_var_name)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return "";
        }

        /*
        try
        {
            String var = (String)runspace.SessionStateProxy.GetVariable(mng_var_name);
            return var;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        */
        return "";
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        /*
        try
        {
            runspace.SessionStateProxy.SetVariable(mng_var_name, value);
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        */

        return (IntPtr)0;
    }


    public static IntPtr DoString(String expression, String inAction = "DoString")
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        // create a pipeline and feed it the script text
        // pipeline = runspace.CreatePipeline();
        // pipeline.Commands.AddScript(expression);

        // add an extra command to transform the script output objects into nicely formatted strings
        // remove this line to get the actual objects that the script returns. For example, the script
        // "Get-Process" returns a collection of System.Diagnostics.Process instances.
        // pipeline.Commands.Add("Out-String");

        try
        {
            /*
            // execute the script
            Collection<PSObject> results = pipeline.Invoke();

            if (pipeline.Error.Count > 0)
            {
                StringBuilder err = new StringBuilder();
                while (!pipeline.Error.EndOfPipeline)
                {
                    var x = pipeline.Error.Read();
                    if (x is Collection<ErrorRecord>)
                    {
                        throw (x as Collection<ErrorRecord>).First().Exception;
                    }
                    if (x is ErrorRecord)
                    {
                        throw (x as ErrorRecord).Exception;
                    }
                    err.AppendLine(pipeline.Error.Read().ToString());
                }
                System.Diagnostics.Trace.WriteLine("Error executing PowerShell Command : " + err);
            }
            */
        }
        catch (Exception err) {
            System.Diagnostics.Trace.WriteLine(err.Message);
        }

        return (IntPtr)0;
    }

    public static IntPtr DoFile(String filename)
    {
        /*
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            using (System.IO.StreamReader sr = new System.IO.StreamReader(filename, System.Text.Encoding.GetEncoding(codepage)))
            {
                String expression = sr.ReadToEnd();
                sr.Close();
                return DoString(expression, filename);
            }
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        */
        return (IntPtr)0;
    }

    public static IntPtr DestroyScope()
    {
        /*
        if (runspace != null)
        {
            // FreeLibrary直前の関数の呼び出し。
            try
            {
                //if (runspace.SessionStateProxy.GetVariable("DestroyScope") != null)
                //{
                DoString("DestroyScope", "DestroyScope");
                //}
            }
            catch (Exception)
            {
            }
            runspace.Dispose();
            runspace = null;
        }
        */
        SetCodePage((IntPtr)default_codepage);
        iDllBindHandle = 0;
        tmpVar = null;

        return (IntPtr)0;
    }

}

