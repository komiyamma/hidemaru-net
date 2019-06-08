using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using Microsoft.ClearScript.V8;

namespace Microsoft.ClearScript.Test
{
    public static class ClearScriptConsole
    {
        static V8ScriptEngine engine;
        static Hidemaru hm;

        private static Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            try
            {
                var requestingAssembly = args.RequestingAssembly;
                var requestedAssembly = new AssemblyName(args.Name);
                var targetName = requestedAssembly.Name + ".dll";
                if (System.IO.File.Exists(targetName))
                {
                    return Assembly.LoadFile(targetName);
                }
            }
            catch
            {
                return null;
            }
            return null;
        }

        public static void Main()
        {
            AppDomain.CurrentDomain.AssemblyResolve += CurrentDomain_AssemblyResolve;

            using (engine = new V8ScriptEngine(typeof(ClearScriptConsole).Name, V8ScriptEngineFlags.EnableDebugging))
            {
                engine.AddHostObject("host", new ExtendedHostFunctions());
                engine.AddHostObject("clr", HostItemFlags.GlobalMembers, new HostTypeCollection("mscorlib", "System", "System.Core", "ClearScript"));

                hm = new Hidemaru();
                engine.AddHostType("hm", typeof(Hidemaru));
                console = new hmV8Console();
                engine.AddHostType("console", typeof(hmV8Console));

                engine.AllowReflection = true;

                // 特別にR関数を追加
                String expression = @"
                function R(text){
                    return text.toString().split(""\n"").slice(1,-1).join(""\n"");
                }
                ";
                engine.Execute(expression);


                RunStartupFile(engine);
                RunConsole(engine);
            }

            AppDomain.CurrentDomain.AssemblyResolve -= CurrentDomain_AssemblyResolve;

        }

        private static void RunStartupFile(ScriptEngine engine)
        {
            try
            {
                var dirPath = Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName);
                if (dirPath != null)
                {
                    var fileName = Path.ChangeExtension("Startup", engine.FileNameExtension);
                    var filePath = Path.Combine(dirPath, fileName);
                    if (File.Exists(filePath))
                    {
                        engine.Execute(fileName, File.ReadAllText(filePath));
                    }
                }
            }
            catch (Exception exception)
            {
                Console.WriteLine("Error: {0}", exception.GetBaseException().Message);
            }
        }

        private static void RunConsole(ScriptEngine engine)
        {
            while (true)
            {
                Console.Write("-> ");

                var command = Console.ReadLine();
                if (command == null)
                {
                    break;
                }

                if (command.Trim().Length < 1)
                {
                    continue;
                }

                try
                {
                    var result = engine.ExecuteCommand(command);
                    if (result != null)
                    {
                        Console.WriteLine(result);
                    }
                }
                catch (Exception exception)
                {
                    Console.WriteLine("Error: {0}", exception.GetBaseException().Message);
                }
            }
        }

        public class Hidemaru
        {

            // debuginfo関数
            public static String debuginfo(params Object[] expressions)
            {
                List<String> list = new List<String>();
                foreach (var exp in expressions)
                {
                    // V8エンジンのオブジェクトであれば、そのまま出しても意味が無いので…
                    if (exp.GetType().Name == "V8ScriptItem")
                    {
                        dynamic dexp = exp;

                        // JSON.stringifyで変換できるはずだ
                        String strify = "";
                        try
                        {
                            strify = engine.Script.JSON.stringify(dexp);
                            list.Add(strify);
                        }
                        catch (Exception)
                        {

                        }

                        // JSON.stringfyで空っぽだったようだ。
                        if (strify == String.Empty)
                        {
                            try
                            {
                                // ECMAScriptのtoString()で変換出来るはずだ…
                                list.Add(dexp.toString());
                            }
                            catch (Exception)
                            {
                                // 変換できなかったら、とりあえず、しょうがないので.NETのToStringで。多分意味はない。
                                list.Add(exp.ToString());
                            }
                        }
                    }

                    // V8オブジェクトでないなら、普通にToString
                    else
                    {
                        list.Add(exp.ToString());
                    }
                }

                String joind = String.Join(" ", list);
                return joind;
            }
        }

        public static String OutputDebugStream(string error)
        {
            return error;
        }

        public static hmV8Console console;

        public class hmV8Console
        {
            public static String log(params Object[] expressions)
            {
                return Hidemaru.debuginfo(FormatObjects(expressions));
            }

            public static String dir(params Object[] expressions)
            {
                return Hidemaru.debuginfo(FormatObjects(expressions));
            }

            public static String debug(params Object[] expressions)
            {
                return Hidemaru.debuginfo(FormatObjects(expressions));
            }

            public static String error(params Object[] expressions)
            {
                return Hidemaru.debuginfo(FormatObjects(expressions));
            }

            public static String clear(params Object[] expressions)
            {
                return Hidemaru.debuginfo("\n\n");
            }

            public static String info(params Object[] expressions)
            {
                return Hidemaru.debuginfo(FormatObjects(expressions));
            }

            public static String trace(params Object[] expressions)
            {
                return Hidemaru.debuginfo(FormatObjects(expressions));
            }

            public static String warn(params Object[] expressions)
            {
                return Hidemaru.debuginfo(expressions);
            }

            public Undefined _console_undef;
            public static String assert()
            {
                return Hidemaru.debuginfo("Assertion failed: console.assert");
            }
            public static String assert(Object assert_exp, params Object[] expressions)
            {
                // 最初が「JavaScript解釈で」False
                if (engine.Script.Boolean(assert_exp) == false)
                {
                    if (expressions.Length > 0)
                    {
                        object[] formattedItems1 = FormatObjects(expressions);
                        object[] formattedItems2 = new object[formattedItems1.Length + 1];
                        formattedItems2[0] = "Assertion failed:";
                        Array.Copy(formattedItems1, 0, formattedItems2, 1, formattedItems1.Length);
                        return Hidemaru.debuginfo(formattedItems2);
                    }
                    else
                    {
                        return assert();
                    }
                }
                return "[Undefined]";
            }

            /// <summary>
            /// console.log系が搭載しているformat相当。
            /// console.logは元々sprintf系の全部を実装しているわけではなく、ほんの一部だけ持っている。
            ///  %s	 String
            ///  %d, %i	 Integer
            ///  %f	 Floating point number
            ///  %o	 Object hyperlink
            /// </summary>
            /// <param name="items"> 書式化指定子の引数達。但し、数が不足していたり、数が多かったりしてもOK </param>
            /// <returns> An array containing formatted strings interspersed with objects. </returns>
            private static object[] FormatObjects(object[] items)
            {
                if (items.Length == 0)
                {
                    return new object[0];
                }
                var result = new List<object>();
                var formattedString = new System.Text.StringBuilder();

                // 最初の引数が文字列なら, 書式化指定子を含んでいる(かもしれない)とされる.
                int itemsConsumed = 1;
                if (items[0] is string)
                {
                    string formatString = (string)items[0];

                    int previousPatternIndex = 0, patternIndex;
                    while (items.Length > itemsConsumed)
                    {
                        // %の書式化の発見.
                        patternIndex = formatString.IndexOf('%', previousPatternIndex);
                        if (patternIndex == -1 || patternIndex == formatString.Length - 1)
                        {
                            break;
                        }

                        // %含まないならそのまま足しこみ.
                        formattedString.Append(formatString, previousPatternIndex, patternIndex - previousPatternIndex);

                        // パターンのタイプ.
                        char patternType = formatString[patternIndex + 1];

                        // 置き換え文字列を決定する
                        string replacement;
                        switch (patternType)
                        {
                            case 's':
                                replacement = items[itemsConsumed].ToString();
                                itemsConsumed++;
                                break;
                            case 'd':
                            case 'i':
                                replacement = hmTypeConverter.ToInteger(items[itemsConsumed]).ToString();
                                itemsConsumed++;
                                break;
                            case 'f':
                                replacement = hmTypeConverter.ToDouble(items[itemsConsumed]).ToString();
                                itemsConsumed++;
                                break;
                            case '%':
                                replacement = "%";
                                break;
                            case 'o':
                                replacement = string.Empty;
                                if (formattedString.Length > 0)
                                    result.Add(formattedString.ToString());
                                result.Add(items[itemsConsumed]);
                                formattedString.Remove(0, formattedString.Length);
                                itemsConsumed++;
                                break;
                            default:
                                replacement = "%" + patternType;
                                break;
                        }

                        // パターンで置き換えたものを出す.
                        formattedString.Append(replacement);

                        // パターンは文字分なので、次のサーチのために２文字進める.
                        previousPatternIndex = patternIndex + 2;
                    }

                    // 結果に%含まないなら、そのまま足しこみ.
                    formattedString.Append(formatString, previousPatternIndex, formatString.Length - previousPatternIndex);

                    // 書式化で整った文字列を配列へと足し込む.
                    if (formattedString.Length > 0)
                    {
                        result.Add(formattedString.ToString());
                    }

                    // パターンによって消費されなかった配列は、そのまま足しこんでおく。C系のprintfと違って数が足りなかったりしたからといってぶっ飛ぶことはない.
                    for (int i = itemsConsumed; i < items.Length; i++)
                    {
                        result.Add(items[i]);
                    }
                    return result.ToArray();
                }
                else
                {
                    // 最初が文字列でないなら、この関数でやることはないい。配列郡全体を何も変化させず、そのまま返す。
                    return items;
                }
            }

        }
    }
}

internal class hmTypeConverter
{
    public static Object ToInteger(Object value)
    {
        // 非数値(+の無限と-の無限を足したり、など)
        if (value is Double)
        {
            double d = (double)value;
            if (Double.IsNaN(d))
            {
                return Double.NaN;
            }
            // プラスの無限
            if (Double.IsPositiveInfinity(d))
            {
                return "Infinity";
            }
            // マイナスの無限
            if (Double.IsNegativeInfinity(d))
            {
                return "-Infinity";
            }
        }

        Object result = new Object();

        // 32bit
        if (IntPtr.Size == 4)
        {
            // まずは整数でトライ
            Int32 itmp = 0;
            bool success = Int32.TryParse(value.ToString(), out itmp);

            if (success == true)
            {
                result = itmp;
            }

            else
            {
                // 次に少数でトライ
                Double dtmp = 0;
                success = Double.TryParse(value.ToString(), out dtmp);
                if (success)
                {
                    // ★★★v8エンジンだと、切り捨て
                    result = (Int32)Math.Floor(dtmp);
                }

                else
                {
                    result = value;
                }
            }
        }

        // 64bit
        else
        {
            // まずは整数でトライ
            Int64 itmp = 0;
            bool success = Int64.TryParse(value.ToString(), out itmp);

            if (success == true)
            {
                result = itmp;
            }

            else
            {
                // 次に少数でトライ
                Double dtmp = 0;
                success = Double.TryParse(value.ToString(), out dtmp);
                if (success)
                {
                    // ★★★v8エンジンだと、切り捨て
                    result = (Int64)Math.Floor(dtmp);
                }
                else
                {
                    result = value;
                }
            }
        }

        return result;

    }

    public static Object ToDouble(Object value)
    {
        // 非数値(+の無限と-の無限を足したり、など)
        if (value is Double)
        {
            double d = (double)value;
            if (Double.IsNaN(d))
            {
                return Double.NaN;
            }
            // プラスの無限
            if (Double.IsPositiveInfinity(d))
            {
                return "Infinity";
            }
            // マイナスの無限
            if (Double.IsNegativeInfinity(d))
            {
                return "-Infinity";
            }
        }

        Object result = new Object();

        // 32bit
        if (IntPtr.Size == 4)
        {
            // まずに少数でトライ
            Double dtmp = 0;
            bool success = Double.TryParse(value.ToString(), out dtmp);
            if (success)
            {
                result = dtmp;
            }
            else
            {
                // 次に整数でトライ
                Int32 itmp = 0;
                success = Int32.TryParse(value.ToString(), out itmp);

                if (success == true)
                {
                    result = itmp;
                }
                else
                {
                    result = value;
                }
            }
        }

        // 64bit
        else
        {
            // まず少数でトライ
            Double dtmp = 0;
            bool success = Double.TryParse(value.ToString(), out dtmp);
            if (success)
            {
                result = dtmp;
            }

            else
            {
                // 次に整数でトライ
                Int64 itmp = 0;
                success = Int64.TryParse(value.ToString(), out itmp);

                if (success == true)
                {
                    result = itmp;
                }
                else
                {
                    result = value;
                }
            }
        }

        return result;
    }

}


