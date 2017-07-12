using System;

namespace Hidemaru
{
    public static class Hm
    {
        public static IntPtr WindowHandle
        {
            get
            {
                return hmNETDynamicLib.Hidemaru.WindowHandle;
            }
        }

        public static Double Version
        {
            get {
                return hmNETDynamicLib.Hidemaru.version;
            }
        }

        public static class Macro
        {
            public static int Eval(String expression)
            {
                return hmNETDynamicLib.Hidemaru.Macro.Eval(expression);
            }

            public static TVar Var = new TVar();
            public class TVar {
                public Object this[String var_name] {
                    get
                    {
                        return hmNETDynamicLib.Hidemaru.Macro.Var[var_name];
                    }
                    set
                    {
                        hmNETDynamicLib.Hidemaru.Macro.Var[var_name] = value;
                    }
                }
            }
        }

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
                get {
                    return hmNETDynamicLib.Hidemaru.Edit.LineText;
                }

                set
                {
                    hmNETDynamicLib.Hidemaru.Edit.LineText = value;
                }

            }

            public static (int LineNo, int Column) CursorPos
            {
                get {
                    var pos = hmNETDynamicLib.Hidemaru.Edit.CursorPos;
                    return (pos.lineno, pos.column);
                }
            }

            public static (int LineNo, int Column, int X, int Y) MousePos
            {
                get {
                    var pos = hmNETDynamicLib.Hidemaru.Edit.MousePos;
                    return (pos.lineno, pos.column, pos.x, pos.y);
                }
            }

        }
    }
}
