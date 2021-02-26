/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Reflection;
using Neo.IronLua;


// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
public class ILmStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmLmDynamicLib.OutputDebugStream(error);
    }

    public static IntPtr CreateScope()
    {
        return hmLmDynamicLib.CreateScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmLmDynamicLib.BindDllHandle(dll);
    }

    public static void SetCodePage(IntPtr cp)
    {
        hmLmDynamicLib.SetCodePage(cp);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmLmDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmLmDynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------
    public static IntPtr GetNumVar(String mng_var_name)
    {
        return hmLmDynamicLib.GetNumVar(mng_var_name);
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        return hmLmDynamicLib.SetNumVar(mng_var_name, value);
    }

    public static String GetStrVar(String mng_var_name)
    {
        return hmLmDynamicLib.GetStrVar(mng_var_name);
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        return hmLmDynamicLib.SetStrVar(mng_var_name, value);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmLmDynamicLib.GetNumItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetNumItemOfList(String mng_arr_name, IntPtr index, IntPtr value)
    {
        return hmLmDynamicLib.SetNumItemOfList(mng_arr_name, index, value);
    }

    public static String GetStrItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmLmDynamicLib.GetStrItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetStrItemOfList(String mng_arr_name, IntPtr index, String value)
    {
        return hmLmDynamicLib.SetStrItemOfList(mng_arr_name, index, value);
    }
    //-----------------------------------------------------------------


    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfDict(String mng_arr_name, String key)
    {
        return hmLmDynamicLib.GetNumItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetNumItemOfDict(String mng_arr_name, String key, IntPtr value)
    {
        return hmLmDynamicLib.SetNumItemOfDict(mng_arr_name, key, value);
    }

    public static String GetStrItemOfDict(String mng_arr_name, String key)
    {
        return hmLmDynamicLib.GetStrItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetStrItemOfDict(String mng_arr_name, String key, String value)
    {
        return hmLmDynamicLib.SetStrItemOfDict(mng_arr_name, key, value);
    }
    //-----------------------------------------------------------------

    public static IntPtr DoString(string expression)
    {
        return hmLmDynamicLib.DoString(expression);
    }

    public static IntPtr DoFile(string filename)
    {
        return hmLmDynamicLib.DoFile(filename);
    }

    public static IntPtr DestroyScope()
    {
        return hmLmDynamicLib.DestroyScope();
    }

}






// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmLmDynamicLib
{
    static hmLmDynamicLib()
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


    public static Lua l;
    public static LuaGlobal g;

    public static void OutputDebugStream(String error)
    {
        System.Diagnostics.Trace.WriteLine(error);
    }

    // ----------------------------------------------------------------
    static Hidemaru hm;

    static readonly String strDllFullPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
    // loadllした時の、変数の値。(そのマクロ内で何番めにloaddllしたかで値が変わる)
    static int iDllBindHandle = 0; // 1以上の整数。hmでloaddllした時の値
    public static IntPtr BindDllHandle(IntPtr dll)
    {
        iDllBindHandle = dll.ToInt32();
        return dll;
    }

    // dllのloaddllタイプによって、渡されたcmd(=expression)に対して、「dllの番号,」or「」を当てはめる処理
    private static String ModifyFuncCallByDllType(String cmd)
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

    // ----------------------------------------------------------------
    private static DllPathResolver dpr;

    public static IntPtr CreateScope()
    {
        // まだエンジンが作られていなければ
        if (l == null || g == null)
        {
            try
            {
                //エンジン作成
                l = new Lua();
                g = l.CreateEnvironment();

                hm = new Hidemaru();
                g.RegisterPackage("hm", typeof(Hidemaru));
                dpr = new DllPathResolver();
                // アセンブリのローダのショートカット
                g["load_assembly"] = new Func<String, Assembly>(asmname => System.Reflection.Assembly.Load(asmname));

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
            Object var = g[mng_var_name];
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
                g[mng_var_name] = value.ToInt32();
            }
            else
            {
                g[mng_var_name] = value.ToInt64();
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
            Object var = g[mng_var_name];
            return var.ToString();
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
            g[mng_var_name] = value;
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
            var arr = (LuaTable)g[mng_arr_name];

            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[index.ToInt32()];

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
            var arr = (LuaTable)g[mng_arr_name];

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
            var arr = (LuaTable)g[mng_arr_name];

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
            var arr = (LuaTable)g[mng_arr_name];

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
            var arr = (LuaTable)g[mng_arr_name];
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
            var arr = (LuaTable)g[mng_arr_name];

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
            var arr = (LuaTable)g[mng_arr_name];
            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[key];
            // string型にできると信じるものである。
            return Convert.ToString(result);
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
            var arr = (LuaTable)g[mng_arr_name];
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
            LuaResult r = g.DoChunk(expression, "test.lua");
            return (IntPtr)1;
        }
        catch (LuaParseException e)
        {
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream("lineno:" + e.Line + " in " + inAction);

            // エラーの行番号
            int ix = e.Line - 1;

            // ソースを改行で分割
            string[] list = expression.Split('\n');

            // 該当行をデバッグモニターへと出力
            if (0 <= ix && ix < list.Length)
            {
                OutputDebugStream(list[ix]);
            }
        }
        catch (LuaRuntimeException e)
        {
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream("lineno:" + e.Line + " in " + inAction);

            // エラーの行番号
            int ix = e.Line - 1;

            // ソースを改行で分割
            string[] list = expression.Split('\n');

            // 該当行をデバッグモニターへと出力
            if (0 <= ix && ix < list.Length)
            {
                OutputDebugStream(list[ix]);
            }
        }
        catch (LuaException e)
        {
            OutputDebugStream(e.GetType().Name + ":");
            OutputDebugStream("lineno:" + e.Line + " in " + inAction);

            // エラーの行番号
            int ix = e.Line - 1;

            // ソースを改行で分割
            string[] list = expression.Split('\n');

            // 該当行をデバッグモニターへと出力
            if (0 <= ix && ix < list.Length)
            {
                OutputDebugStream(list[ix]);
            }
        }
        catch (Exception e)
        {
            OutputDebugStream(e.GetType().Name+":");
            OutputDebugStream(e.Message);
            OutputDebugStream(e.StackTrace);
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
        if (l != null)
        {
            // FreeLibrary直前の関数の呼び出し。
            try
            {
                DoString("if DestroyScope then DestroyScope() end", "DestroyScope");
            }
            catch (Exception)
            {
            }
            g.Clear();
            l.Dispose();
            l = null;
            dpr = null;
        }

        SetCodePage((IntPtr)default_codepage);
        iDllBindHandle = 0;
        tmpVar = null;

        return (IntPtr)0;
    }

}

