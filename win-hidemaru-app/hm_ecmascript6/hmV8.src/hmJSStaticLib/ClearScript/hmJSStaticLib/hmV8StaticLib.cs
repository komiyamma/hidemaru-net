using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Microsoft.ClearScript;
using Microsoft.ClearScript.V8;
using System.IO;
using hmV8DynamicNS;

// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
public class IV8StaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmV8DynamicLib.OutputDebugStream(error);
    }

    public static IntPtr SetDebuggingPort(IntPtr port)
    {
        return hmV8DynamicLib.SetDebuggingPort(port);
    }

    public static IntPtr CreateScope()
    {
        return hmV8DynamicLib.CreateScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmV8DynamicLib.BindDllHandle(dll);
    }

    public static void SetCodePage(IntPtr cp)
    {
        hmV8DynamicLib.SetCodePage(cp);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmV8DynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmV8DynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------
    public static IntPtr GetNumVar(String mng_var_name)
    {
        return hmV8DynamicLib.GetNumVar(mng_var_name);
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        return hmV8DynamicLib.SetNumVar(mng_var_name, value);
    }

    public static String GetStrVar(String mng_var_name)
    {
        return hmV8DynamicLib.GetStrVar(mng_var_name);
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        return hmV8DynamicLib.SetStrVar(mng_var_name, value);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmV8DynamicLib.GetNumItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetNumItemOfList(String mng_arr_name, IntPtr index, IntPtr value)
    {
        return hmV8DynamicLib.SetNumItemOfList(mng_arr_name, index, value);
    }

    public static String GetStrItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmV8DynamicLib.GetStrItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetStrItemOfList(String mng_arr_name, IntPtr index, String value)
    {
        return hmV8DynamicLib.SetStrItemOfList(mng_arr_name, index, value);
    }
    //-----------------------------------------------------------------


    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfDict(String mng_arr_name, String key)
    {
        return hmV8DynamicLib.GetNumItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetNumItemOfDict(String mng_arr_name, String key, IntPtr value)
    {
        return hmV8DynamicLib.SetNumItemOfDict(mng_arr_name, key, value);
    }

    public static String GetStrItemOfDict(String mng_arr_name, String key)
    {
        return hmV8DynamicLib.GetStrItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetStrItemOfDict(String mng_arr_name, String key, String value)
    {
        return hmV8DynamicLib.SetStrItemOfDict(mng_arr_name, key, value);
    }
    //-----------------------------------------------------------------

    public static IntPtr DoString(string expression)
    {
        return hmV8DynamicLib.DoString(expression);
    }

    public static IntPtr DoFile(string filename)
    {
        return hmV8DynamicLib.DoFile(filename);
    }

    public static IntPtr DestroyScope()
    {
        return hmV8DynamicLib.DestroyScope();
    }

}





// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmV8DynamicLib
{
    static hmV8DynamicLib()
    {
        StringBuilder exe_full_path = new StringBuilder(260);
        GetModuleFileName(IntPtr.Zero, exe_full_path, (ulong)exe_full_path.Capacity);
        strExecuteFullpath = exe_full_path.ToString();
    }

    static String strExecuteFullpath;
    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern ulong GetModuleFileName(IntPtr hModule, StringBuilder filename, ulong bufsize);

    public static Microsoft.ClearScript.V8.V8ScriptEngine engine;
    public static V8EngineCore core;

    public static void OutputDebugStream(string error)
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

    private static int iDebuggingPort = 0;
    public static IntPtr SetDebuggingPort( IntPtr port )
    {
        iDebuggingPort = port.ToInt32();
        return (IntPtr)1;
    }

    private static DllPathResolver dpr;

    private static V8EngineCore CreateCore()
    {
        var pathResolver = new ModulePathResolver("", new[] { ".js", ".json", ".dll" }, "index");
        var loaderFactory = new ModuleLoaderFactory();

        core = new V8EngineCore(pathResolver, loaderFactory, iDebuggingPort);
        return core;
    }

    //----------------------------------------------------------------------------------------------
    public static IntPtr CreateScope()
    {
        // まだエンジンが作られていなければ
        if (engine == null)
        {
            try
            {
                core = CreateCore();
                engine = core.Engine;
                core.ExposeGlobalRequire(); // requireが使えるように

                hm = new Hidemaru();
                engine.AddHostType("hm", typeof(Hidemaru));
                console = new hmV8Console();
                engine.AddHostType("console", typeof(hmV8Console));

                // ①ヒアドキュメント用の関数
                // ②:空のDestory関数
                String expression = @"
                function R(text){
                    return text.toString().match(/\/\*([\s\S]*)\*\//)[1].toString();
                }

                hm.Macro.Var = new Proxy(()=>{}, {
                    apply: function(target, that, args) {
                        if (args.length > 1 ) {
                            return hm.Macro.__Var(args[0], args[1]);
                        }
                        else if (args.length == 1 ) {
                            return hm.Macro.__Var(args[0]);
                        }
                    },
                    get(target, prop, receiver) {
                        return hm.Macro.__Var(prop);
                    },
                    set(target, prop, val, receiver) {
                        return hm.Macro.__Var(prop, val);
                    }
                }
                )
                ";

                engine.Execute(expression);

                engine.Script.DestroyScope = new Action(() => { });

                dpr = new DllPathResolver();
                engine.AddHostObject("AssemblyPath", new List<String>());

                // ヒアドキュメント用の関数 R(text)
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

            Object result = arr[index.ToInt32()];

            // 要求に従い[ ]でアクセスしてみる。
            if (IntPtr.Size == 4)
            {
                // 要求に従い[ ]でアクセスしてみる。
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

            /*
            var errorLocation = engine.GetErrorLocation(error);
            if (!string.IsNullOrWhiteSpace(errorLocation))
            {
                message += "\n" + errorLocation;
            }

            var stackTrace = WindowsScriptEngine.
            if (!string.IsNullOrWhiteSpace(stackTrace))
            {
                message += "\n" + stackTrace;
            }

            return message;
            */


            ScriptEngineException next = e.InnerException as ScriptEngineException;
            while (next != null)
            {
                /*
                OutputDebugStream(next.Data.ToString());
                OutputDebugStream(next.Source.ToString());
                 */
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
                OutputDebugStream(next.Data.ToString());
                OutputDebugStream(next.Source.ToString());
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
                DoString("DestroyScope()", "DestroyScope");
            }
            catch (Exception)
            {
            }
            engine = null;
        }

        SetCodePage((IntPtr)default_codepage);
        iDllBindHandle = 0;
        tmpVar = null;
        iDebuggingPort = 0;

        dpr = null;

        core.Dispose();
        core = null;

        return (IntPtr)0;
    }

}

