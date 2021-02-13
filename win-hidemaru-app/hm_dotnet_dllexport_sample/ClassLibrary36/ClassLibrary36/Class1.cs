using System;
using System.Runtime.InteropServices;

namespace ClassLibrary36
{

    public class Class1
    {
        private static String dynamic_func(String a, String b)
        {
            return a + b;
        }

        private static IntPtr dynamic_func(IntPtr a, IntPtr b)
        {
            return (IntPtr)((long)a + (long)b);
        }

        static StaticWStrPtrHandle returnStringObject = null;

        [DllExport]
        public unsafe static IntPtr abc(IntPtr a, IntPtr b)
        {
            // 1番目の引数(=a)の型
            Hidemaru_DllFuncParamType param_type = (Hidemaru_DllFuncParamType)Hidemaru_GetDllFuncCalledType(1);
            object arg1 = null;
            object arg2 = null;

            if (param_type == Hidemaru_DllFuncParamType.INT || param_type == Hidemaru_DllFuncParamType.DOUBLE)
            {
                arg1 = (IntPtr)a;
            }
            else if (param_type == Hidemaru_DllFuncParamType.WCHAR_PTR)
            {
                arg1 = (String)Marshal.PtrToStringUni(a);
            }

            // 2番目の引数(=b)の型
            param_type = (Hidemaru_DllFuncParamType)Hidemaru_GetDllFuncCalledType(2);
            if (param_type == Hidemaru_DllFuncParamType.INT || param_type == Hidemaru_DllFuncParamType.DOUBLE)
            {
                arg2 = (IntPtr)b;
            }
            else if (param_type == Hidemaru_DllFuncParamType.WCHAR_PTR)
            {
                arg2 = (String)Marshal.PtrToStringUni(b);
            }


            object my_ret = null;
            // どっちも数値の場合に呼ぶ関数
            if (arg1 is IntPtr && arg2 is IntPtr)
            {
                my_ret = dynamic_func((IntPtr)arg1, (IntPtr)arg2);

            // どちらかが文字列の場合に呼ぶ関数
            } else
            {
                my_ret = dynamic_func(arg1.ToString(), arg2.ToString());
            }

            // 返り値として要求されている型をチェック
            Hidemaru_DllFuncReturnType rtn_type = (Hidemaru_DllFuncReturnType)Hidemaru_GetDllFuncCalledType(0);

            if (rtn_type == Hidemaru_DllFuncReturnType.INT || rtn_type == Hidemaru_DllFuncReturnType.DOUBLE)
            {
                Decimal result = 0;
                bool success = Decimal.TryParse(my_ret.ToString(), out result);
                if (success)
                {
                    // オーバーフローなどのチェックは今回省略
                    return (IntPtr)result;
                }

                if (my_ret is String)
                {
                    // 文字列でパースに失敗したら、返すものもんないので、文字列の長さでもかえしておこうか。
                    return (IntPtr)(my_ret as String).Length;
                }

            }
            else if (rtn_type == Hidemaru_DllFuncReturnType.WCHAR_PTR)
            {
                returnStringObject?.Dispose();
                returnStringObject = new StaticWStrPtrHandle(my_ret.ToString());
                return returnStringObject.DangerousGetHandle();
            }

            return (IntPtr)0;
        }

        enum Hidemaru_DllFuncReturnType
        {
            NOTCALL = 0, //呼ばれていない
            INT = 1, // 数値型を返すべき(dllfuncまたはdllfuncw)
            CHAR_PTR = 2,// char *を返すべき(dllfuncstr)
            WCHAR_PTR = 3, // WCHAR*を返すべき(dllfuncstrw)
            DOUBLE = 4 // doubleを返すべき(dllfuncまたはdllfuncwで浮動小数点数が有効)
        };
        enum Hidemaru_DllFuncParamType
        {
            NOPARAM = 0,
            INT = 0x0100, // 数値型の引数
            CHAR_PTR = 0x0200,// char *の引数
            WCHAR_PTR = 0x0300, // WCHAR*の引数
            DOUBLE = 0x0400 // doubleの引数
        };


        [DllImport("Hidemaru.exe", CallingConvention = CallingConvention.Winapi)]
        extern static int Hidemaru_GetDllFuncCalledType(int n);
    }
}