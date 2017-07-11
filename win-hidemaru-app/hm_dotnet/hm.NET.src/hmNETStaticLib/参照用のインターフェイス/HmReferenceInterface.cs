using System;

namespace Hidemaru
{
    public static class Hm
    {
        public static IntPtr GetWindowHandle()
        {
            return hmNETDynamicLib.Hidemaru.WindowHandle;
        }

        public static Double GetVersion()
        {
            return hmNETDynamicLib.Hidemaru.version;
        }

        public class Macro
        {
            public static int DoEval(String expression)
            {
                return hmNETDynamicLib.Hidemaru.Macro.Eval(expression);
            }
            public static Object GetVar(String var_name)
            {
                return hmNETDynamicLib.Hidemaru.Macro.Var[var_name];
            }
            public static Boolean SetVar(String var_name, Object value)
            {
                hmNETDynamicLib.Hidemaru.Macro.Var[var_name] = value;
                return true;
            }
        }

        public static class Edit
        {
            public static String GetFileFullPath()
            {
                String file_name = hmNETDynamicLib.Hidemaru.Edit.FileName;
                if (String.IsNullOrEmpty(file_name))
                {
                    return null;
                } else {
                    return file_name;
                }
            }

            public static String GetTotalText()
            {
                return hmNETDynamicLib.Hidemaru.Edit.TotalText;
            }
            public static Boolean SetTotalText(String text)
            {
                hmNETDynamicLib.Hidemaru.Edit.TotalText = text;
                return true;
            }
            public static String GetSelectedText()
            {
                return hmNETDynamicLib.Hidemaru.Edit.SelectedText;
            }
            public static Boolean SetSelectedText(String text)
            {
                hmNETDynamicLib.Hidemaru.Edit.SelectedText = text;
                return true;
            }
            public static String GetLineText()
            {
                return hmNETDynamicLib.Hidemaru.Edit.LineText;
            }
            public static Boolean SetLineText(String text)
            {
                hmNETDynamicLib.Hidemaru.Edit.LineText = text;
                return true;
            }

            public static (int LineNo, int Column) GetCursorPos()
            {
                var pos = hmNETDynamicLib.Hidemaru.Edit.CursorPos;
                return (pos.lineno, pos.column);
            }

            public static (int LineNo, int Column) GetCursorPosFromMousePos()
            {
                var pos = hmNETDynamicLib.Hidemaru.Edit.CursorPosFromMousePos;
                return (pos.lineno, pos.column);
            }

        }
    }
}
