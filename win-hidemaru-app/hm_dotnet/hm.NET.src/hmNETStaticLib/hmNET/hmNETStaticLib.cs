/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Reflection;


// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
internal class INETStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmNETDynamicLib.OutputDebugStream(error);
    }

    public static void TraceMethodInfo(String assm_path, String class_name, String method_name)
    {
        hmNETDynamicLib.TraceMethodInfo(assm_path, class_name, method_name);
    }

    public static void TraceExceptionInfo(Exception e)
    {
        hmNETDynamicLib.TraceExceptionInfo(e);
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

    public static Object SubCallMethod(String assm_path, String class_name, String method_name, List<Object> args, String func_mode)
    {
        return hmNETDynamicLib.SubCallMethod(assm_path, class_name, method_name, args, func_mode);
    }

    public static IntPtr DetachScope(IntPtr status)
    {
        return hmNETDynamicLib.DetachScope(status);
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
    public static void TraceMethodInfo(String assm_path, String class_name, String method_name)
    {
        System.Diagnostics.Trace.WriteLine("アセンブリパス   :" + assm_path);
        System.Diagnostics.Trace.WriteLine("名前空間.クラス名:" + class_name);
        System.Diagnostics.Trace.WriteLine("メソッド名       :" + method_name);
    }
    public static void TraceExceptionInfo(Exception e)
    {
        System.Diagnostics.Trace.WriteLine(e.GetType());
        System.Diagnostics.Trace.WriteLine(e.Message);
        System.Diagnostics.Trace.WriteLine(e.StackTrace);
    }

    [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wParam, StringBuilder lParam);
    [DllImport("user32.dll", EntryPoint = "SendMessage", CharSet = CharSet.Auto)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, StringBuilder wParam, StringBuilder lParam);
    [DllImport("user32.dll", SetLastError = true)]
    public static extern bool SendMessage(IntPtr hWnd, uint Msg, IntPtr wparam, IntPtr lparam);
    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, int command, IntPtr lparam);

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

        return (IntPtr)1;
    }

    public static Object SubCallMethod(String assm_path, String class_name, String method_name, List<Object> args, String func_mode)
    {
        Exception method_ex = null;
        try
        {
            Assembly assm = null;
            Type t = null;

            if (assm_path.Length > 0)
            {
                assm = Assembly.LoadFile(assm_path);
                if (assm == null)
                {
                    System.Diagnostics.Trace.WriteLine("ロード出来ない");
                }
                else
                {
                    // System::Diagnostics::Trace::WriteLine(assm->FullName);
                }

                foreach (Type t2 in assm.GetExportedTypes())
                {
                    if (t2.ToString() == class_name)
                    {
                        t = assm.GetType(class_name);
                    }
                }
            }
            else
            {
                t = Type.GetType(class_name);
            }
            if (t == null)
            {
                System.Diagnostics.Trace.WriteLine("MissingMethodException(クラスもしくはメソッドを見つけることが出来ない):");
                TraceMethodInfo(assm_path, class_name, method_name);
                return null;
            }

            // メソッドの定義タイプを探る。
            MethodInfo m;
            try
            {
                m = t.GetMethod(method_name);
            }
            catch (Exception ex)
            {
                // 基本コースだと一致してない系の可能性やオーバーロードなど未解決エラーを控えておく
                // t->GetMethod(...)は論理的には不要だが、「エラー情報のときにわかりやすい情報を.NETに自動で出力してもらう」ためにダミーで呼び出しておく
                method_ex = ex;

                // オーバーロードなら1つに解決できるように型情報も含めてmは上書き
                List<Type> args_types = new List<Type>();
                foreach (var arg in args)
                {
                    args_types.Add(arg.GetType());
                }
                m = t.GetMethod(method_name, args_types.ToArray());
            }

            Object o = null;
            try
            {
                // デタッチ関数の時に、引数が無いパターンを許す
                if (func_mode == "detach_func_mode")
                {
                    var paramInfo = m.GetParameters();
                    // 受け側の破棄メソッドの「引数が無い」ならば
                    if (paramInfo.Length == 0)
                    {
                        o = m.Invoke(null, null);
                        // System::Diagnostics::Trace::WriteLine("引数無し");
                    }
                    // 引数があるならば、一応その値を渡す。とは言っても基本的にアセンブリなのでアプリ終了時にしかこれは実行されないハズであるが…
                    else
                    {
                        o = m.Invoke(null, args.ToArray());
                        // System::Diagnostics::Trace::WriteLine("引数有り");
                    }
                }
                else
                {
                    o = m.Invoke(null, args.ToArray());
                }
            }
            catch (Exception)
            {
                System.Diagnostics.Trace.WriteLine("指定のメソッドの実行時、例外が発生しました。");
                throw;
            }
            return o;
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine("指定のアセンブリやメソッドを特定する前に、例外が発生しました。");
            TraceMethodInfo(assm_path, class_name, method_name);
            if (method_ex != null)
            {
                TraceExceptionInfo(method_ex);
            }
            TraceExceptionInfo(e);
        }


        return null;
    }

    public static IntPtr DetachScope(IntPtr status)
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

        return (IntPtr)1;
    }

}

