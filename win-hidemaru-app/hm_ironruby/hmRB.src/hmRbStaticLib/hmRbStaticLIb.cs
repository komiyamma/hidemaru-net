/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using System;
using System.Text;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;
using Microsoft.Scripting;
using Microsoft.Scripting.Hosting;

// ★内部でdynamic型を利用しないもの。C++リンク用途のため「だけの」「コンパイルによってメソッド数が変化しない」インターフェイス。
// このようなスタブを用意することで、C++とリンクすることが可能となる(=メソッドの個数がC#とC++/CLIで一致させることが出来る)
public class IRbStaticLib
{
    public static void OutputDebugStream(String error)
    {
        hmRbDynamicLib.OutputDebugStream(error);
    }

    public static IntPtr CreateScope()
    {
        return hmRbDynamicLib.CreateScope();
    }

    public static IntPtr BindDllHandle(IntPtr dll)
    {
        return hmRbDynamicLib.BindDllHandle(dll);
    }

    public static void SetCodePage(IntPtr cp)
    {
        hmRbDynamicLib.SetCodePage(cp);
    }

    public static IntPtr SetTmpVar(Object value)
    {
        return hmRbDynamicLib.SetTmpVar(value);
    }

    public static Object PopTmpVar()
    {
        return hmRbDynamicLib.PopTmpVar();
    }

    //-----------------------------------------------------------------
    public static IntPtr GetNumVar(String mng_var_name)
    {
        return hmRbDynamicLib.GetNumVar(mng_var_name);
    }

    public static IntPtr SetNumVar(String mng_var_name, IntPtr value)
    {
        return hmRbDynamicLib.SetNumVar(mng_var_name, value);
    }

    public static String GetStrVar(String mng_var_name)
    {
        return hmRbDynamicLib.GetStrVar(mng_var_name);
    }

    public static IntPtr SetStrVar(String mng_var_name, String value)
    {
        return hmRbDynamicLib.SetStrVar(mng_var_name, value);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmRbDynamicLib.GetNumItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetNumItemOfList(String mng_arr_name, IntPtr index, IntPtr value)
    {
        return hmRbDynamicLib.SetNumItemOfList(mng_arr_name, index, value);
    }

    public static String GetStrItemOfList(String mng_arr_name, IntPtr index)
    {
        return hmRbDynamicLib.GetStrItemOfList(mng_arr_name, index);
    }

    public static IntPtr SetStrItemOfList(String mng_arr_name, IntPtr index, String value)
    {
        return hmRbDynamicLib.SetStrItemOfList(mng_arr_name, index, value);
    }
    //-----------------------------------------------------------------


    //-----------------------------------------------------------------
    public static IntPtr GetNumItemOfDict(String mng_arr_name, String key)
    {
        return hmRbDynamicLib.GetNumItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetNumItemOfDict(String mng_arr_name, String key, IntPtr value)
    {
        return hmRbDynamicLib.SetNumItemOfDict(mng_arr_name, key, value);
    }

    public static String GetStrItemOfDict(String mng_arr_name, String key)
    {
        return hmRbDynamicLib.GetStrItemOfDict(mng_arr_name, key);
    }

    public static IntPtr SetStrItemOfDict(String mng_arr_name, String key, String value)
    {
        return hmRbDynamicLib.SetStrItemOfDict(mng_arr_name, key, value);
    }
    //-----------------------------------------------------------------

    public static IntPtr DoString(string expression)
    {
        return hmRbDynamicLib.DoString(expression);
    }

    public static IntPtr DoFile(string filename)
    {
        return hmRbDynamicLib.DoFile(filename);
    }

    public static IntPtr DestroyScope()
    {
        return hmRbDynamicLib.DestroyScope();
    }

}




// IronRubyのエラーリスナーを独自に定義
public sealed class IronRubyErrorListener : ErrorListener
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
public sealed partial class hmRbDynamicLib
{
    static hmRbDynamicLib()
    {
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

    public static ScriptEngine re;
    public static ScriptScope ss;

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

    const String tmp = "____";

    public static IntPtr CreateScope()
    {
        // まだエンジンが作られていなければ
        if (re == null || ss == null)
        {
            try
            {
                //エンジン作成
                re = IronRuby.Ruby.CreateEngine(opt => { opt.ExceptionDetail = true; });
                ss = re.CreateScope();

                hm = new Hidemaru();
                ss.SetVariable("hm" + tmp, hm);

                String code =
                    "$hm = self.hm" + tmp + "\n" +
                    "def DestroyScope() end\n";

                // 8.66以上の時は、自分自身をロードしたマクロファイルのディレクトリを$LOAD_PATHに追加しておく。
                // マネージの.dllが簡単にロード出来るように。
                // hmPyのようなICollectionを使うやり方だとIronRubyは上手くいかない。
                if (hm.version >= 866)
                {
                    code += "$LOAD_PATH.push($hm.Macro.Var['currentmacrodirectory'].to_s);\n";
                }

                ScriptSource source = re.CreateScriptSourceFromString(code);
                source.Execute(ss);

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
            Object var = re.Execute(mng_var_name, ss);
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
            // self.$abc とかにするとエラーが出るので、self.doll_abc_ などの名前にするための工夫
            String mng_var_name_doll = mng_var_name + ""; // コピー
            mng_var_name_doll = mng_var_name_doll.Replace("$", "doll_") + tmp;
            if (IntPtr.Size == 4)
            {
                ss.SetVariable(mng_var_name, value.ToInt32());
            }
            else
            {
                ss.SetVariable(mng_var_name, value.ToInt64());
            }
            ScriptSource source = re.CreateScriptSourceFromString(mng_var_name + "=" + "self." + mng_var_name_doll);
            source.Execute(ss);
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
            Object var = re.Execute<String>(mng_var_name, ss);
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
            // self.$abc とかにするとエラーが出るので、self.doll_abc_ などの名前にするための工夫
            String mng_var_name_doll = mng_var_name + ""; // コピー
            mng_var_name_doll = mng_var_name_doll.Replace("$", "doll_") + tmp;
            ss.SetVariable(mng_var_name_doll, value);
            ScriptSource source = re.CreateScriptSourceFromString(mng_var_name + "=" + "self." + mng_var_name_doll);
            source.Execute(ss);
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
            dynamic arr = re.Execute(mng_arr_name, ss);

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
            dynamic arr = re.Execute(mng_arr_name, ss);

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
            dynamic arr = re.Execute(mng_arr_name, ss);

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
            dynamic arr = re.Execute(mng_arr_name, ss);

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
            dynamic arr = re.Execute(mng_arr_name, ss);
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
            dynamic arr = re.Execute(mng_arr_name, ss);

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
            dynamic arr = re.Execute(mng_arr_name, ss);

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
            dynamic arr = re.Execute(mng_arr_name, ss);
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

    public static IntPtr DoString(String expression, String inLocation = "DoString")
    {
        if (CreateScope() == (IntPtr)0)
        {
            return (IntPtr)0;
        }


        // ここで強制的に１行足されているので、
        // この変なのが必要なのは、
        // $hm.debuginfo(<<-'MACRO'
        //     message("OK");
        // MACRO
        // );

        // 後でLineで１行分数値引いている
        String patch_expression_code = "$hm._('--');";
        expression = patch_expression_code + "\n" + expression;

        try
        {

            // 文字列からソース生成
            ScriptSource source = re.CreateScriptSourceFromString(expression, "RubyEvalDoString", SourceCodeKind.File);

            // 独自定義のエラーリスナーをエラー出力先に設定
            IronRubyErrorListener error = new IronRubyErrorListener();

            // コンパイルしてみる。エラーがあれば、IronRubyErrorsへ
            source.Compile(error);
            // 事前コンパイルでエラーがあった
            if (error.ErrorCode != 0)
            {
                // エラーの位置をデバッグモニターへ
                // -1しているのは、上で$hm._(...)している分
                OutputDebugStream(string.Format("error description {0} \r\nlineno:{1} column:{2} in {3}", error.Message, error.Span.Start.Line - 1, error.Span.Start.Column, inLocation));

                // エラーの行番号
                // -1ではなく-2なのは、上で$hm._(...)している分
                int ix = error.Span.Start.Line - 2;

                // ソースを改行で分割
                string[] expression_lines = expression.Split('\n');

                // 該当行をデバッグモニターへと出力(ある程度範囲を持たせて)
                int[] bound = { -1, 0, 1 };

                foreach (var b in bound)
                {
                    // 範囲チェック
                    if (0 <= ix + b && ix + b < expression_lines.Length)
                    {
                        var error_expression_line = expression_lines[ix + b];
                        // ダミー行以外で、有効なトークンを含んだ行である
                        if (!error_expression_line.Contains(patch_expression_code) && !String.IsNullOrWhiteSpace(error_expression_line) && error_expression_line.Length > 1)
                        {
                            OutputDebugStream(error_expression_line);
                        }
                    }
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
            try
            {
                // IronRubyではIronPythonのようにトレースエラーがいい感じでは取れないので、
                // 以下のもので代行する。
                /*
                OutputDebugStream(e.GetType().Name + ":");
                OutputDebugStream(e.Message);
                OutputDebugStream(e.StackTrace);

                var ruby_exception_data = IronRuby.Runtime.RubyExceptionData.GetInstance(e);
                var ruby_exception_data_bt_list = ruby_exception_data.Backtrace;
                */

                var error_op = re.GetService<ExceptionOperations>();
                var stack_frame_errors = error_op.FormatException(e);
                // ソースを改行で分割
                string[] stack_frame_lines = stack_frame_errors.Split('\n');

                String str_lineno = "";
                foreach (var el in stack_frame_lines)
                {
                    Regex r = new Regex(@"RubyEvalDoString:(\d+)");
                    Match m = r.Match(el);
                    if (m.Success)
                    {
                        str_lineno = m.Groups[1].Value;
                        if (str_lineno.Length > 0)
                        {
                            int ix = Int32.Parse(str_lineno) - 2;
                            // ソースを改行で分割
                            string[] expression_lines = expression.Split('\n');

                            OutputDebugStream(string.Format("error description {0} \r\nlineno:{1} in {2}", e.GetType(), (ix + 1), inLocation));

                            // 該当行をデバッグモニターへと出力(ある程度範囲を持たせて)
                            int[] bound = { -1, 0, 1 };

                            foreach (var b in bound)
                            {
                                // 範囲チェック
                                if (0 <= ix + b && ix + b < expression_lines.Length)
                                {
                                    var error_expression_line = expression_lines[ix + b];
                                    // ダミー行以外で、有効なトークンを含んだ行である
                                    if (!error_expression_line.Contains(patch_expression_code) && !String.IsNullOrWhiteSpace(error_expression_line) && error_expression_line.Length > 1)
                                    {
                                        OutputDebugStream(error_expression_line);
                                    }
                                }
                            }
                        }

                    }
                }

                foreach (var el in stack_frame_lines)
                {
                    Regex r = new Regex(@"^(.*)(RubyEvalDoString:)(\d+)");
                    Match m = r.Match(el);
                    if (m.Success)
                    {
                        str_lineno = m.Groups[3].Value;
                        if (str_lineno.Length > 0)
                        {
                            int ix = Int32.Parse(str_lineno) - 1;
                            OutputDebugStream(m.Groups[1] + inLocation + ":" + ix);
                        }

                    }
                    else
                    {
                        OutputDebugStream(el);
                    }
                }

            }
            catch (Exception ee)
            {
                OutputDebugStream(ee.Message);
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
            using (System.IO.StreamReader sr = new System.IO.StreamReader(filename, System.Text.Encoding.GetEncoding(default_codepage)))
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
            try
            {
                // FreeLibrary直前の関数の呼び出し。
                DoString("DestroyScope()", "DestroyScope");
            }
            catch (Exception e)
            {
                OutputDebugStream(e.Message);
            }
            finally
            {
                ss = null;
                re = null;
            }
        }

        SetCodePage((IntPtr)default_codepage);
        iDllBindHandle = 0;
        tmpVar = null;

        return (IntPtr)0;
    }

}

