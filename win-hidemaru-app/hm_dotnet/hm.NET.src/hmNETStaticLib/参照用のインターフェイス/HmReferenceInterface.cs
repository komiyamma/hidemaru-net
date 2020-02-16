﻿/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;

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

            // 実行系
            public interface IExec
            {
                IResult File(String filepath);
                IResult Eval(String expression);
            }

            // 実行系の実態
            private class TExec : IExec
            {
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
            }

            public static IExec Exec = new TExec();


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

            public static IHidemaruStreamReader Open(String filepath, int hm_encode)
            {
                return hmNETDynamicLib.Hidemaru.File.Open(filepath, hm_encode);
            }

            public static IEncoding GetEncoding(String filepath)
            {
                return hmNETDynamicLib.Hidemaru.File.GetEncoding(filepath);
            }
        }
    }
}

