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

                // ���ʂ�R�֐���ǉ�
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

            // debuginfo�֐�
            public static String debuginfo(params Object[] expressions)
            {
                List<String> list = new List<String>();
                foreach (var exp in expressions)
                {
                    // V8�G���W���̃I�u�W�F�N�g�ł���΁A���̂܂܏o���Ă��Ӗ��������̂Łc
                    if (exp.GetType().Name == "V8ScriptItem")
                    {
                        dynamic dexp = exp;

                        // JSON.stringify�ŕϊ��ł���͂���
                        String strify = "";
                        try
                        {
                            strify = engine.Script.JSON.stringify(dexp);
                            list.Add(strify);
                        }
                        catch (Exception)
                        {

                        }

                        // JSON.stringfy�ŋ���ۂ������悤���B
                        if (strify == String.Empty)
                        {
                            try
                            {
                                // ECMAScript��toString()�ŕϊ��o����͂����c
                                list.Add(dexp.toString());
                            }
                            catch (Exception)
                            {
                                // �ϊ��ł��Ȃ�������A�Ƃ肠�����A���傤���Ȃ��̂�.NET��ToString�ŁB�����Ӗ��͂Ȃ��B
                                list.Add(exp.ToString());
                            }
                        }
                    }

                    // V8�I�u�W�F�N�g�łȂ��Ȃ�A���ʂ�ToString
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
                // �ŏ����uJavaScript���߂ŁvFalse
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
            /// console.log�n�����ڂ��Ă���format�����B
            /// console.log�͌��Xsprintf�n�̑S�����������Ă���킯�ł͂Ȃ��A�ق�̈ꕔ���������Ă���B
            ///  %s	 String
            ///  %d, %i	 Integer
            ///  %f	 Floating point number
            ///  %o	 Object hyperlink
            /// </summary>
            /// <param name="items"> �������w��q�̈����B�B�A���A�����s�����Ă�����A�������������肵�Ă�OK </param>
            /// <returns> An array containing formatted strings interspersed with objects. </returns>
            private static object[] FormatObjects(object[] items)
            {
                if (items.Length == 0)
                {
                    return new object[0];
                }
                var result = new List<object>();
                var formattedString = new System.Text.StringBuilder();

                // �ŏ��̈�����������Ȃ�, �������w��q���܂�ł���(��������Ȃ�)�Ƃ����.
                int itemsConsumed = 1;
                if (items[0] is string)
                {
                    string formatString = (string)items[0];

                    int previousPatternIndex = 0, patternIndex;
                    while (items.Length > itemsConsumed)
                    {
                        // %�̏������̔���.
                        patternIndex = formatString.IndexOf('%', previousPatternIndex);
                        if (patternIndex == -1 || patternIndex == formatString.Length - 1)
                        {
                            break;
                        }

                        // %�܂܂Ȃ��Ȃ炻�̂܂ܑ�������.
                        formattedString.Append(formatString, previousPatternIndex, patternIndex - previousPatternIndex);

                        // �p�^�[���̃^�C�v.
                        char patternType = formatString[patternIndex + 1];

                        // �u����������������肷��
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

                        // �p�^�[���Œu�����������̂��o��.
                        formattedString.Append(replacement);

                        // �p�^�[���͕������Ȃ̂ŁA���̃T�[�`�̂��߂ɂQ�����i�߂�.
                        previousPatternIndex = patternIndex + 2;
                    }

                    // ���ʂ�%�܂܂Ȃ��Ȃ�A���̂܂ܑ�������.
                    formattedString.Append(formatString, previousPatternIndex, formatString.Length - previousPatternIndex);

                    // �������Ő������������z��ւƑ�������.
                    if (formattedString.Length > 0)
                    {
                        result.Add(formattedString.ToString());
                    }

                    // �p�^�[���ɂ���ď����Ȃ������z��́A���̂܂ܑ�������ł����BC�n��printf�ƈ���Đ�������Ȃ������肵������Ƃ����ĂԂ���Ԃ��Ƃ͂Ȃ�.
                    for (int i = itemsConsumed; i < items.Length; i++)
                    {
                        result.Add(items[i]);
                    }
                    return result.ToArray();
                }
                else
                {
                    // �ŏ���������łȂ��Ȃ�A���̊֐��ł�邱�Ƃ͂Ȃ����B�z��S�S�̂������ω��������A���̂܂ܕԂ��B
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
        // �񐔒l(+�̖�����-�̖����𑫂�����A�Ȃ�)
        if (value is Double)
        {
            double d = (double)value;
            if (Double.IsNaN(d))
            {
                return Double.NaN;
            }
            // �v���X�̖���
            if (Double.IsPositiveInfinity(d))
            {
                return "Infinity";
            }
            // �}�C�i�X�̖���
            if (Double.IsNegativeInfinity(d))
            {
                return "-Infinity";
            }
        }

        Object result = new Object();

        // 32bit
        if (IntPtr.Size == 4)
        {
            // �܂��͐����Ńg���C
            Int32 itmp = 0;
            bool success = Int32.TryParse(value.ToString(), out itmp);

            if (success == true)
            {
                result = itmp;
            }

            else
            {
                // ���ɏ����Ńg���C
                Double dtmp = 0;
                success = Double.TryParse(value.ToString(), out dtmp);
                if (success)
                {
                    // ������v8�G���W�����ƁA�؂�̂�
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
            // �܂��͐����Ńg���C
            Int64 itmp = 0;
            bool success = Int64.TryParse(value.ToString(), out itmp);

            if (success == true)
            {
                result = itmp;
            }

            else
            {
                // ���ɏ����Ńg���C
                Double dtmp = 0;
                success = Double.TryParse(value.ToString(), out dtmp);
                if (success)
                {
                    // ������v8�G���W�����ƁA�؂�̂�
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
        // �񐔒l(+�̖�����-�̖����𑫂�����A�Ȃ�)
        if (value is Double)
        {
            double d = (double)value;
            if (Double.IsNaN(d))
            {
                return Double.NaN;
            }
            // �v���X�̖���
            if (Double.IsPositiveInfinity(d))
            {
                return "Infinity";
            }
            // �}�C�i�X�̖���
            if (Double.IsNegativeInfinity(d))
            {
                return "-Infinity";
            }
        }

        Object result = new Object();

        // 32bit
        if (IntPtr.Size == 4)
        {
            // �܂��ɏ����Ńg���C
            Double dtmp = 0;
            bool success = Double.TryParse(value.ToString(), out dtmp);
            if (success)
            {
                result = dtmp;
            }
            else
            {
                // ���ɐ����Ńg���C
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
            // �܂������Ńg���C
            Double dtmp = 0;
            bool success = Double.TryParse(value.ToString(), out dtmp);
            if (success)
            {
                result = dtmp;
            }

            else
            {
                // ���ɐ����Ńg���C
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


