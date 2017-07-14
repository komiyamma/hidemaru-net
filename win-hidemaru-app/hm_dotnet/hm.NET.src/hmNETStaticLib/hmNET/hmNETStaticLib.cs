using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;


// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
internal class INETStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmNETDynamicLib.OutputDebugStream(error);
    }

    public static IntPtr AttachScope()
    {
        return hmNETDynamicLib.AttachScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmNETDynamicLib.BindDllHandle(dll);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmNETDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmNETDynamicLib.PopTmpVar();
    }



    public static IntPtr CallMethod(string expression)
    {
        return hmNETDynamicLib.CallMethod(expression);
    }

    public static IntPtr DetachScope()
    {
        return hmNETDynamicLib.DetachScope();
    }

}



// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
internal partial class hmNETDynamicLib
{
    static Object engine = null;

    static hmNETDynamicLib()
    {
        StringBuilder exe_full_path = new StringBuilder(260);
        GetModuleFileName(IntPtr.Zero, exe_full_path, (ulong)exe_full_path.Capacity);
        strExecuteFullpath = exe_full_path.ToString();
    }

    static String strExecuteFullpath;
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern ulong GetModuleFileName(IntPtr hModule, StringBuilder filename, ulong bufsize);

    public static void OutputDebugStream(String error)
    {
        System.Diagnostics.Trace.WriteLine(error);
    }

    [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, int wParam, StringBuilder lParam);
    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr SendMessage(int hWnd, int Msg, int wparam, int lparam);

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

    private static DllPathResolver dpr;

    private static Hidemaru hm;

    //----------------------------------------------------------------------------------------------
    public static IntPtr AttachScope()
    {
        // まだエンジンが作られていなければ
        if (engine == null)
        {
            try
            {
                // Standardモードのコンプライアンスまで引き上げる
                engine = new Object();

                hm = new Hidemaru();

                dpr = new DllPathResolver();

                return (IntPtr)1;
            }
            catch (Exception e)
            {
                OutputDebugStream(e.Message);
                return (IntPtr)0;
            }

        }
        return (IntPtr)1;
    }

    public static IntPtr CallMethod(String expression, String inAction = "DoString")
    {
        if (AttachScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
        }
        catch (Exception e)
        {
            OutputDebugStream("in " + inAction);
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream(e.Message);
        }

        return (IntPtr)0;
    }

    public static IntPtr DetachScope()
    {
        if (engine != null)
        {
            // FreeLibrary直前の関数の呼び出し。
            try
            {
                CallMethod("DestroyScope()", "DestroyScope");
            }
            catch (Exception)
            {
            }
            engine = null;
        }

        iDllBindHandle = 0;
        tmpVar = null;

        dpr = null;

        return (IntPtr)0;
    }

}

