/* 
 * Copyright (c) 2016-2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using IronPython.Hosting;
using Microsoft.Scripting;
using Microsoft.Scripting.Hosting;


// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
public class IPyStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmPyDynamicLib.OutputDebugStream(error);
    }

    public static IntPtr CreateScope()
    {
        return hmPyDynamicLib.CreateScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmPyDynamicLib.BindDllHandle(dll);
    }

    public static void SetCodePage(IntPtr cp)
    {
        hmPyDynamicLib.SetCodePage(cp);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmPyDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmPyDynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------
    public static IntPtr GetNumVar(String mng_var_name)
    {
        return hmPyDynamicLib.GetNumVar(mng_var_name);
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        return hmPyDynamicLib.SetNumVar(mng_var_name, value);
    }

    public static String GetStrVar(String mng_var_name)
    {
        return hmPyDynamicLib.GetStrVar(mng_var_name);
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        return hmPyDynamicLib.SetStrVar(mng_var_name, value);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmPyDynamicLib.GetNumItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetNumItemOfList(String mng_arr_name, IntPtr index, IntPtr value)
    {
        return hmPyDynamicLib.SetNumItemOfList(mng_arr_name, index, value);
    }

    public static String GetStrItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmPyDynamicLib.GetStrItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetStrItemOfList(String mng_arr_name, IntPtr index, String value)
    {
        return hmPyDynamicLib.SetStrItemOfList(mng_arr_name, index, value);
    }
    //-----------------------------------------------------------------


    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfDict(String mng_arr_name, String key)
    {
        return hmPyDynamicLib.GetNumItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetNumItemOfDict(String mng_arr_name, String key, IntPtr value)
    {
        return hmPyDynamicLib.SetNumItemOfDict(mng_arr_name, key, value);
    }

    public static String GetStrItemOfDict(String mng_arr_name, String key)
    {
        return hmPyDynamicLib.GetStrItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetStrItemOfDict(String mng_arr_name, String key, String value)
    {
        return hmPyDynamicLib.SetStrItemOfDict(mng_arr_name, key, value);
    }
    //-----------------------------------------------------------------

    public static IntPtr DoString(string expression)
    {
        return hmPyDynamicLib.DoString(expression);
    }

    public static IntPtr DoFile(string filename)
    {
        return hmPyDynamicLib.DoFile(filename);
    }

    public static IntPtr DestroyScope()
    {
        return hmPyDynamicLib.DestroyScope();
    }

}

// IronPythonのエラーリスナーを独自に定義
public sealed class IronPytnonErrorListener : ErrorListener
{
    public string Message { get; set; }
    public int ErrorCode { get; set; }
    public Severity sev { get; set; }
    public SourceSpan Span { get; set; }
    public override void ErrorReported(ScriptSource source, string message, Microsoft.Scripting.SourceSpan span, int errorCode, Microsoft.Scripting.Severity severity)
    {
        Message = message;
        ErrorCode = errorCode;
        sev = severity;
        Span = span;
    }
}


// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmPyDynamicLib
{
    static hmPyDynamicLib() {
        StringBuilder exe_full_path = new StringBuilder(260);
        GetModuleFileName(IntPtr.Zero, exe_full_path, exe_full_path.Capacity);
        strExecuteFullpath = exe_full_path.ToString();
    }

    static String strExecuteFullpath;
    [DllImport("kernel32.dll", SetLastError = true)]
    [PreserveSig]
    private static extern uint GetModuleFileName([In]IntPtr hModule, [Out] StringBuilder lpFilename,  [In][MarshalAs(UnmanagedType.U4)]int nSize);

    [DllImport("user32.dll", SetLastError = true)]
    private static extern IntPtr SendMessage(IntPtr hWnd, uint Msg, int command, IntPtr lparam);

    public static ScriptEngine pe;
    public static ScriptScope ss;

    public static void OutputDebugStream(String error) {
        System.Diagnostics.Trace.WriteLine(error);
    }

    // hmのため
    //----------------------------------------------------------------------------------------------
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

    //----------------------------------------------------------------------------------------------
    public static IntPtr CreateScope()
    {
        // まだエンジンが作られていなければ
        if (pe == null || ss == null)
        {
            try
            {
                //エンジン作成
                pe = Python.CreateEngine();
                ss = pe.CreateScope();
                hm = new Hidemaru();
                // __builtin__モジュールの変数として、これをセッティング。グローバル変数として参照可能となる。
                pe.GetBuiltinModule().SetVariable("hm", hm);
                // ss.SetVariable("hm", hm);

                // マクロのパスを足す
                SetCurrentMacroDirectory();

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

    // 8.66以上の時は、自分自身をロードしたマクロファイルのディレクトリをIronPythonのエンジンのpathに追加しておく。
    // マネージの.dllが簡単にロード出来るように。
    private static void SetCurrentMacroDirectory()
    {
        if (Hidemaru.version >= 866)
        {
            String currentmacrodirectory = (String)Hidemaru.Macro.Var["currentmacrodirectory"];

            ICollection<String> paths = pe.GetSearchPaths();
            if (!String.IsNullOrWhiteSpace(currentmacrodirectory))
            {
                paths.Add(currentmacrodirectory);
                pe.SetSearchPaths(paths);
            }
        }
    }

    const int default_codepage = 932;
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
            Object var = ss.GetVariable(mng_var_name);
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
            if (IntPtr.Size == 4) {
                ss.SetVariable(mng_var_name, value.ToInt32());
            } else {
                ss.SetVariable(mng_var_name, value.ToInt64());
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
            Object var = ss.GetVariable(mng_var_name);
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
            ss.SetVariable(mng_var_name, value);
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
            dynamic arr = ss.GetVariable(mng_arr_name);
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
            dynamic arr = ss.GetVariable(mng_arr_name);

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
            dynamic arr = ss.GetVariable(mng_arr_name);

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
            dynamic arr = ss.GetVariable(mng_arr_name);

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
            dynamic arr = ss.GetVariable(mng_arr_name);
            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[key];
            // int型にできると信じるものである。
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
            dynamic arr = ss.GetVariable(mng_arr_name);

            if (IntPtr.Size == 4) { 
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
            dynamic arr = ss.GetVariable(mng_arr_name);

            // 要求に従い[ ]でアクセスしてみる。
            Object result = arr[key];

            // int型にできると信じるものである。
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
            dynamic arr = ss.GetVariable(mng_arr_name);
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
            ScriptSource source = pe.CreateScriptSourceFromString(expression);

            // 独自定義のエラーリスナーをエラー出力先に設定
            IronPytnonErrorListener error = new IronPytnonErrorListener();

            // コンパイルしてみる。エラーがあれば、IronRubyErrorsへ
            source.Compile(error);
            // 事前コンパイルでエラーがあった
            if (error.ErrorCode != 0)
            {
                // エラーの位置をデバッグモニターへ
                OutputDebugStream(string.Format("error description:\n {0} \nlineno:{1} column:{2} in {3}", error.Message, error.Span.Start.Line, error.Span.Start.Column, inAction));

                // エラーの行番号
                int ix = error.Span.Start.Line - 1;

                // ソースを改行で分割
                string[] list = expression.Split('\n');

                // 該当行をデバッグモニターへと出力
                if (0 <= ix && ix < list.Length)
                {
                    OutputDebugStream(list[ix]);
                }

            }

            // コンパイルエラーがなければ
            if (error.ErrorCode == 0)
            {
                // 実行
                source.Execute(ss);
            }
            return (IntPtr)1;
        }
        catch (Exception e)
        {
            // エラーを出すための処理がさらに危ないかもしれない
            try
            {
                OutputDebugStream(e.GetType().Name + ":");
                OutputDebugStream(e.Message);

                // スタックエラーの一覧
                var stack = IronPython.Runtime.Operations.PythonOps.GetDynamicStackFrames(e);

                // expressionを行で分割して配列に
                string[] codelines = expression.Split('\n');

                OutputDebugStream("error description:");

                // 各々のエラー情報に対して
                foreach (var s in stack)
                {
                    // とりあえずそんままエラー出力
                    OutputDebugStream(s.ToString());

                    // ラインナンバーはあるか？
                    int? lineno = s.GetFileLineNumber();

                    // あれば
                    if (lineno.HasValue)
                    {
                        // 出力
                        OutputDebugStream("lineno:" + lineno + " in " + inAction);

                        // 特定の関数から呼ばれたものでなければ(例えばDestroyScopeとかから呼ばれたものでなければ)
                        if (inAction != "DestroyScope")
                        {
                            // 番号が範囲内であれば
                            if (0 <= lineno.Value - 1 && lineno.Value < codelines.Length)
                            {
                                // 該当の行を表示(エラーの箇所がわかりやすくなる)
                                OutputDebugStream(codelines[lineno.Value - 1]);
                            }
                        }
                    }

                    // expression内ではなく、ファイルになっているようなら
                    String filename = s.GetFileName();
                    if (filename != "<string>")
                    {
                        // ファイル名も出す
                        OutputDebugStream("filename:" + filename);
                    }
                }
            }
            catch (Exception)
            {

            }
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
        if (ss != null)
        {
            // FreeLibrary直前の関数の呼び出し。
            try
            {
                dynamic fnDestroyScope;
                bool exists = ss.TryGetVariable("DestroyScope", out fnDestroyScope);
                if (exists)
                {
                    // fnDestroyScope();
                    DoString("if DestroyScope: DestroyScope()", "DestroyScope");
                }
            }
            catch (Exception)
            {
            }
            ss = null;
            pe = null;
        }

        SetCodePage((IntPtr)default_codepage);
        iDllBindHandle = 0;
        tmpVar = null;
 
        return (IntPtr)0;
    }

}

