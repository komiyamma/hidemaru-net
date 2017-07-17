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
            get
            {
                return hmNETDynamicLib.Hidemaru.version;
            }
        }

        public static class Macro
        {
            public static int Eval(String expression)
            {
                return hmNETDynamicLib.Hidemaru.Macro.Eval(expression);
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
    }
}

