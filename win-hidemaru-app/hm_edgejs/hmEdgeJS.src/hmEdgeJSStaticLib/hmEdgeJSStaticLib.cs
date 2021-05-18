/* 
 * Copyright (c) 2016-2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Collections.ObjectModel;
using System.Threading;


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

    public static IntPtr SetTmpVar(Object value)
    {
        return hmEdgeJSDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmEdgeJSDynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------


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
            GetModuleFileName(IntPtr.Zero, exe_full_path, exe_full_path.Capacity);
            strExecuteFullpath = exe_full_path.ToString();
            InitMethodReference();
        } catch(Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

    static String strExecuteFullpath;
    [DllImport("kernel32.dll", SetLastError = true)]
    [PreserveSig]
    private static extern uint GetModuleFileName([In]IntPtr hModule, [Out] StringBuilder lpFilename,  [In][MarshalAs(UnmanagedType.U4)]int nSize);


    [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, StringBuilder lParam);
    [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, StringBuilder wParam, StringBuilder lParam);
    [DllImport("user32.dll", SetLastError = true)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wparam, IntPtr lparam);
    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, int command, IntPtr lparam);

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
        return (IntPtr)1;
    }

    static ManualResetEvent manualResetEvent;

    public static IntPtr DoFile(String filename)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            manualResetEvent = new ManualResetEvent(false);
            var tsk = InitExpression(filename);
            manualResetEvent.WaitOne();
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            manualResetEvent.Set();
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static IntPtr DestroyScope()
    {
        iDllBindHandle = 0;
        tmpVar = null;

        return (IntPtr)0;
    }

}

