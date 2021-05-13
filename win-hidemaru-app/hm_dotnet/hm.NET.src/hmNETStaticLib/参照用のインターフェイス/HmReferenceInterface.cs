/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.Collections.Generic;
using System.Dynamic;

namespace Hidemaru
{
    public static class Hm
    {
        // メインのウィンドウ
        public static IntPtr WindowHandle
        {
            get
            {
                return hmNETDynamicLib.Hidemaru.WindowHandle;
            }
        }

        // バージョン
        public static Double Version
        {
            get
            {
                return hmNETDynamicLib.Hidemaru.version;
            }
        }

        // マクロ空間
        public static class Macro
        {
            // 実行中か
            public static bool IsExecuting
            {
                get
                {
                    return hmNETDynamicLib.Hidemaru.Macro.IsExecuting;
                }
            }

            // マクロでの問い合わせ結果系
            public interface IResult
            {
                int Result { get; }
                String Message { get; }
                Exception Error { get; }
            }

            // 問い合わせ結果系の実態。外から見えないように
            private class TResult : IResult
            {
                public int Result { get; set; }
                public string Message { get; set; }
                public Exception Error { get; set; }

                public TResult(int Result, String Message, Exception Error)
                {
                    this.Result = Result;
                    this.Message = Message;
                    this.Error = Error;
                }
            }

            // マクロでの問い合わせ結果系
            public interface IStatementResult
            {
                int Result { get; }
                String Message { get; }
                Exception Error { get; }
                List<Object> Args { get; }
            }


            private class TStatementResult : IStatementResult
            {
                public int Result { get; set; }
                public string Message { get; set; }
                public Exception Error { get; set; }
                public List<Object> Args { get; set; }

                public TStatementResult(int Result, String Message, Exception Error, List<Object> Args)
                {
                    this.Result = Result;
                    this.Message = Message;
                    this.Error = Error;
                    this.Args = new List<Object>(Args); // コピー渡し
                }
            }

            // マクロでの問い合わせ結果系
            public interface IFunctionResult
            {
                object Result { get; }
                String Message { get; }
                Exception Error { get; }
                List<Object> Args { get; }
            }

            private class TFunctionResult : IFunctionResult
            {
                public object Result { get; set; }
                public string Message { get; set; }
                public Exception Error { get; set; }
                public List<Object> Args { get; set; }

                public TFunctionResult(object Result, String Message, Exception Error, List<Object> Args)
                {
                    this.Result = Result;
                    this.Message = Message;
                    this.Error = Error;
                    this.Args = new List<Object>(Args); // コピー渡し
                }
            }

            // 実行系
            public interface IExec
            {
                IResult File(String filepath);
                IResult Eval(String expression);
                IResult Method(string message_parameter, string dirfullpath, string typefullname, string methodname);
                IResult Method(string message_parameter, Delegate delegate_method);
           }

            // 実行系の実態
            private class TExec : IExec
            {
                private const string V = " is not 'PUBLIC'";

                public IResult Eval(string expression)
                {
                    var ret = hmNETDynamicLib.Hidemaru.Macro.ExecEval(expression);
                    var result = new TResult(ret.Result, ret.Message, ret.Error);
                    return result;
                }

                public IResult File(string filepath)
                {
                    var ret = hmNETDynamicLib.Hidemaru.Macro.ExecFile(filepath);
                    var result = new TResult(ret.Result, ret.Message, ret.Error);
                    return result;
                }

                public IResult Method(string parameter, string dirfullpath, string typefullname, string methodname)
                {
                    var ret = hmNETDynamicLib.Hidemaru.Macro.BornMacroScopeMethod(parameter, dirfullpath, typefullname, methodname);
                    var result = new TResult(ret.Result, ret.Message, ret.Error);
                    return result;
                }

                public IResult Method(string parameter, Delegate delegate_method)
                {
                    if (delegate_method.Method.IsStatic && delegate_method.Method.IsPublic)
                    {
                        var ret = hmNETDynamicLib.Hidemaru.Macro.BornMacroScopeMethod(parameter, delegate_method.Method.DeclaringType.Assembly.Location, delegate_method.Method.DeclaringType.FullName, delegate_method.Method.Name);
                        var result = new TResult(ret.Result, ret.Message, ret.Error);
                        return result;
                    }
                    else if (!delegate_method.Method.IsStatic)
                    {

                        string message_no_static = delegate_method.Method.DeclaringType.FullName + "." + delegate_method.Method.Name + " is not 'STATIC' in " + delegate_method.Method.DeclaringType.Assembly.Location;
                        var result_no_static = new TResult(0, "", new MissingMethodException(message_no_static));
                        System.Diagnostics.Trace.WriteLine(message_no_static);
                        return result_no_static;
                    }
                    else if (!delegate_method.Method.IsPublic)
                    {
                        string message_no_public = delegate_method.Method.DeclaringType.FullName + "." + delegate_method.Method.Name + " is not 'PUBLIC' in " + delegate_method.Method.DeclaringType.Assembly.Location ;
                        var result_no_public = new TResult(0, "", new MissingMethodException(message_no_public));
                        System.Diagnostics.Trace.WriteLine(message_no_public);
                        return result_no_public;
                    }
                    string message_missing = delegate_method.Method.DeclaringType.FullName + "." + delegate_method.Method.Name + "is 'MISSING' access in " + delegate_method.Method.DeclaringType.Assembly.Location;
                    var result_missing = new TResult(0, "", new MissingMethodException(delegate_method.Method.Name + " is missing access"));
                    System.Diagnostics.Trace.WriteLine(result_missing);
                    return result_missing;
                }

            }

            public static IExec Exec = new TExec();


            public static IStatementResult Statement(string funcname, params object[] args)
            {
                var ret = hmNETDynamicLib.Hidemaru.Macro.AsStatementTryInvokeMember(funcname, args);
                IStatementResult result = new TStatementResult(ret.Result, ret.Message, ret.Error, ret.Args);
                return result;
            }

            public static IFunctionResult Function(string funcname, params object[] args)
            {
                var ret = hmNETDynamicLib.Hidemaru.Macro.AsFunctionTryInvokeMember(funcname, args);
                IFunctionResult result = new TFunctionResult(ret.Result, ret.Message, ret.Error, ret.Args);
                return result;
            }

            public static IResult Eval(String expression)
            {
                var ret = hmNETDynamicLib.Hidemaru.Macro.Eval(expression);
                var result = new TResult(ret.Result, ret.Message, ret.Error);
                return result;
            }

            public static IVar Var = new TVar();
            public interface IVar
            {
                Object this[String name] { get; set; }
            }
            private class TVar : IVar
            {
                public Object this[String name]
                {
                    get
                    {
                        return hmNETDynamicLib.Hidemaru.Macro.Var[name];
                    }
                    set
                    {
                        hmNETDynamicLib.Hidemaru.Macro.Var[name] = value;
                    }
                }
            }
        }

        // エディット系
        public static class Edit
        {
            public static String FilePath
            {
                get
                {
                    String file_name = hmNETDynamicLib.Hidemaru.Edit.FileName;
                    if (String.IsNullOrEmpty(file_name))
                    {
                        return null;
                    }
                    else
                    {
                        return file_name;
                    }
                }
            }

            public static bool QueueStatus
            {
                get
                {
                    return hmNETDynamicLib.Hidemaru.Edit.CheckQueueStatus;
                }
            }

            public static String TotalText
            {
                get
                {
                    return hmNETDynamicLib.Hidemaru.Edit.TotalText;
                }
                set
                {
                    hmNETDynamicLib.Hidemaru.Edit.TotalText = value;
                }
            }

            public static String SelectedText
            {
                get
                {
                    return hmNETDynamicLib.Hidemaru.Edit.SelectedText;
                }
                set
                {
                    hmNETDynamicLib.Hidemaru.Edit.SelectedText = value;
                }
            }

            public static String LineText
            {
                get
                {
                    return hmNETDynamicLib.Hidemaru.Edit.LineText;
                }

                set
                {
                    hmNETDynamicLib.Hidemaru.Edit.LineText = value;
                }

            }

            public interface ICursorPos
            {
                int LineNo { get; }
                int Column { get; }
            }
            public static ICursorPos CursorPos
            {
                get
                {
                    var pos = hmNETDynamicLib.Hidemaru.Edit.CursorPos;
                    return pos;
                }
            }

            public interface IMousePos
            {
                int LineNo { get; }
                int Column { get; }
                int X { get; }
                int Y { get; }
            }
            public static IMousePos MousePos
            {
                get
                {
                    var pos = hmNETDynamicLib.Hidemaru.Edit.MousePos;
                    return pos;
                }
            }

        }


        public static class File
        {
            public interface IHidemaruEncoding
            {
                int HmEncode { get; }
            }
            public interface IMicrosoftEncoding
            {
                int MsCodePage { get; }
            }

            public interface IEncoding : IHidemaruEncoding, IMicrosoftEncoding
            {
            }

            public interface IHidemaruStreamReader : IDisposable
            {
                IEncoding Encoding { get; }
                String Read();
                String FilePath { get; }
                void Close();
            }

            public static IHidemaruStreamReader Open(String filepath, int hm_encode = -1)
            {
                return hmNETDynamicLib.Hidemaru.File.Open(filepath, hm_encode);
            }

            public static IEncoding GetEncoding(String filepath)
            {
                return hmNETDynamicLib.Hidemaru.File.GetEncoding(filepath);
            }
        }

        // アウトプットペイン系
        public static class OutputPane
        {
            public static int Output(String message)
            {
                return hmNETDynamicLib.Hidemaru.OutputPane.Output(message);
            }

            public static int Push()
            {
                return hmNETDynamicLib.Hidemaru.OutputPane.Push();
            }

            public static int Pop()
            {
                return hmNETDynamicLib.Hidemaru.OutputPane.Pop();
            }

            public static int Clear()
            {
                return hmNETDynamicLib.Hidemaru.OutputPane.Clear();
            }

            public static IntPtr WindowHandle
            {
                get
                {
                    return hmNETDynamicLib.Hidemaru.OutputPane.WindowHandle;
                }
            }

            public static IntPtr SendMessage(int command_id)
            {
                return hmNETDynamicLib.Hidemaru.OutputPane.SendMessage(command_id);
            }

            public static int SetBaseDir(String dirpath)
            {
                return hmNETDynamicLib.Hidemaru.OutputPane.SetBaseDir(dirpath);
            }
        }
    }
}

