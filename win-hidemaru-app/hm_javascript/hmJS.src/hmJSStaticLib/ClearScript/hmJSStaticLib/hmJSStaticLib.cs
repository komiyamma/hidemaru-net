/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Microsoft.ClearScript;
using Microsoft.ClearScript.Windows;


// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
public class IJSStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmJSDynamicLib.OutputDebugStream(error);
    }

    public static IntPtr CreateScope()
    {
        return hmJSDynamicLib.CreateScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmJSDynamicLib.BindDllHandle(dll);
    }

    public static void SetCodePage(IntPtr cp)
    {
        hmJSDynamicLib.SetCodePage(cp);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmJSDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmJSDynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------
    public static IntPtr GetNumVar(String mng_var_name)
    {
        return hmJSDynamicLib.GetNumVar(mng_var_name);
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        return hmJSDynamicLib.SetNumVar(mng_var_name, value);
    }

    public static String GetStrVar(String mng_var_name)
    {
        return hmJSDynamicLib.GetStrVar(mng_var_name);
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        return hmJSDynamicLib.SetStrVar(mng_var_name, value);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmJSDynamicLib.GetNumItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetNumItemOfList(String mng_arr_name, IntPtr index, IntPtr value)
    {
        return hmJSDynamicLib.SetNumItemOfList(mng_arr_name, index, value);
    }

    public static String GetStrItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmJSDynamicLib.GetStrItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetStrItemOfList(String mng_arr_name, IntPtr index, String value)
    {
        return hmJSDynamicLib.SetStrItemOfList(mng_arr_name, index, value);
    }
    //-----------------------------------------------------------------


    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfDict(String mng_arr_name, String key)
    {
        return hmJSDynamicLib.GetNumItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetNumItemOfDict(String mng_arr_name, String key, IntPtr value)
    {
        return hmJSDynamicLib.SetNumItemOfDict(mng_arr_name, key, value);
    }

    public static String GetStrItemOfDict(String mng_arr_name, String key)
    {
        return hmJSDynamicLib.GetStrItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetStrItemOfDict(String mng_arr_name, String key, String value)
    {
        return hmJSDynamicLib.SetStrItemOfDict(mng_arr_name, key, value);
    }
    //-----------------------------------------------------------------

    public static IntPtr DoString(string expression)
    {
        return hmJSDynamicLib.DoString(expression);
    }

    public static IntPtr DoFile(string filename)
    {
        return hmJSDynamicLib.DoFile(filename);
    }

    public static IntPtr DestroyScope()
    {
        return hmJSDynamicLib.DestroyScope();
    }

}




// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmJSDynamicLib
{
    static hmJSDynamicLib()
    {
        StringBuilder exe_full_path = new StringBuilder(260);
        GetModuleFileName(IntPtr.Zero, exe_full_path, (ulong)exe_full_path.Capacity);
        strExecuteFullpath = exe_full_path.ToString();
    }

    static String strExecuteFullpath;
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern ulong GetModuleFileName(IntPtr hModule, StringBuilder filename, ulong bufsize);

    [DllImport("user32.dll", SetLastError = true)]
    public static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, int command, IntPtr lparam);

    public static Microsoft.ClearScript.Windows.JScriptEngine engine;

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

    private static DllPathResolver dpr;

    //----------------------------------------------------------------------------------------------
    public static IntPtr CreateScope()
    {
        // まだエンジンが作られていなければ
        if (engine == null)
        {
            try
            {
                // Standardモードのコンプライアンスまで引き上げる
                engine = new Microsoft.ClearScript.Windows.JScriptEngine(Microsoft.ClearScript.Windows.WindowsScriptEngineFlags.EnableStandardsMode | Microsoft.ClearScript.Windows.WindowsScriptEngineFlags.EnableDebugging);
                engine.AddHostObject("clr", new HostTypeCollection("mscorlib", "System", "System.Core"));
                engine.AddHostObject("host", new ExtendedHostFunctions());
                engine.AddHostObject("AssemblyPath", new List<String>());

                hm = new Hidemaru();
                engine.AddHostType("hm", typeof(Hidemaru));
                console = new hmJSConsole();
                engine.AddHostType("console", typeof(hmJSConsole));

                // ヒアドキュメント用の関数 R(text)
                String expression = @"
                function R(text){
                    return text.toString().match(/\/\*([\s\S]*)\*\//)[1].toString();
                }
                ";
                engine.Execute(expression);

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

        try
        {
            Object var = engine.Script[mng_var_name];
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
        return (IntPtr)0;
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            if (IntPtr.Size == 4)
            {
                engine.Script[mng_var_name] = value.ToInt32();
            }
            else
            {
                engine.Script[mng_var_name] = value.ToInt64();
            }
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static String GetStrVar(String mng_var_name)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return "";
        }

        try
        {
            String var = (String)engine.Script[mng_var_name];
            return var;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return "";
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            engine.Script[mng_var_name] = value;
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static IntPtr GetNumItemOfList(String mng_arr_name, IntPtr index)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];

            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[index.ToInt32()];

            // 要求に従い[ ]でアクセスしてみる。
            if (IntPtr.Size == 4)
            {
                return (IntPtr)(Convert.ToInt32(result));
            }
            else
            {
                return (IntPtr)(Convert.ToInt64(result));
            }
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static IntPtr SetNumItemOfList(String mng_arr_name, IntPtr index, IntPtr value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];
            // 要求に従い[ ]でアクセスした先に、値を入れてみる
            if (IntPtr.Size == 4)
            {
                // 要求に従い[ ]でアクセスした先に、値を入れてみる
                arr[index.ToInt32()] = value.ToInt32();
            }
            else
            {
                // 要求に従い[ ]でアクセスした先に、値を入れてみる
                arr[index.ToInt32()] = value.ToInt64();
            }

            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static String GetStrItemOfList(String mng_arr_name, IntPtr index)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return "";
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];

            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[index.ToInt32()];

            // int型にできると信じるものである。
            return Convert.ToString(result);
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return "";
    }

    public static IntPtr SetStrItemOfList(String mng_arr_name, IntPtr index, String value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];

            // 要求に従い[ ]でアクセスした先に、値を入れてみる
            arr[index.ToInt32()] = value;

            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static IntPtr GetNumItemOfDict(String mng_arr_name, String key)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];
            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[key];
            // int型にできると信じるものである。
            if (IntPtr.Size == 4)
            {
                return (IntPtr)(Convert.ToInt32(result));
            }
            else
            {
                return (IntPtr)(Convert.ToInt64(result));
            }
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static IntPtr SetNumItemOfDict(String mng_arr_name, String key, IntPtr value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];

            if (IntPtr.Size == 4)
            {
                // 要求に従い[ ]でアクセスした先に、値を入れてみる
                arr[key] = value.ToInt32();
            }
            else
            {
                // 要求に従い[ ]でアクセスした先に、値を入れてみる
                arr[key] = value.ToInt64();
            }
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static String GetStrItemOfDict(String mng_arr_name, String key)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return "";
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];

            // 要求に従い[ ]でアクセスしてみる。
            String result = arr[key];

            // int型にできると信じるものである。
            return result;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return "";
    }
    public static IntPtr SetStrItemOfDict(String mng_arr_name, String key, String value)
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 何型かはわからないが… 呼び出し側は list[x]形式で呼び出せるものだと考えた。
            dynamic arr = engine.Script[mng_arr_name];
            // 要求に従い[ ]でアクセスした先に、値を入れてみる
            arr[key] = value;

            return (IntPtr)1;
        }
        catch (Exception e)
        {
            OutputDebugStream(e.Message);
        }
        return (IntPtr)0;
    }

    public static IntPtr DoString(String expression, String inAction = "DoString")
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }

        try
        {
            // 文字列からソース生成
            engine.Evaluate(expression);
            return (IntPtr)1;
        }
        catch (ScriptEngineException e)
        {
            OutputDebugStream("in " + inAction);
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream(e.ErrorDetails);

            var stack = engine.GetStackTrace();
            OutputDebugStream(stack.ToString());

            ScriptEngineException next = e.InnerException as ScriptEngineException;
            while (next != null)
            {
                OutputDebugStream(next.ErrorDetails);
                next = next.InnerException as ScriptEngineException;
            } 
        }
        catch (ScriptInterruptedException e)
        {
            OutputDebugStream("in " + inAction);
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream(e.ErrorDetails);

            var stack = engine.GetStackTrace();
            OutputDebugStream(stack.ToString());

            ScriptInterruptedException next = e.InnerException as ScriptInterruptedException;
            while (next != null)
            {
                OutputDebugStream(next.ErrorDetails);
                next = next.InnerException as ScriptInterruptedException;
            }
        }
        catch (Exception e)
        {
            OutputDebugStream("in " + inAction);
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream(e.Message);
        }

        return (IntPtr)0;
    }

    public static IntPtr DoFile(String filename)
    {
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
        return (IntPtr)0;
    }

    public static IntPtr DestroyScope()
    {
        if (engine != null)
        {
            // FreeLibrary直前の関数の呼び出し。
            try
            {
                if (engine.Script["DestroyScope"] != null)
                {
                    DoString("DestroyScope()", "DestroyScope");
                }
            }
            catch (Exception)
            {
            }
            engine = null;
        }

        SetCodePage((IntPtr)default_codepage);
        iDllBindHandle = 0;
        tmpVar = null;

        dpr = null;

        return (IntPtr)0;
    }

}

