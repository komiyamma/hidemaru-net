using System;
using System.Runtime.InteropServices;


namespace ClassLibrary15
{
    public class Class1
    {

       // [DllExport]
        public static IntPtr Count(IntPtr a, IntPtr b)
        {
            return a;
        }

       // 文字列を引数とする関数
       // [DllExport]
        public static IntPtr my_func1(IntPtr wStringPointer)
        {
            String str = Marshal.PtrToStringUni(wStringPointer);
            return (IntPtr)str.Length;
        }

        // 文字列を返す関数
        static StaticWStrPtrHandle hReturnWStringPointer = new StaticWStrPtrHandle("");
       // [DllExport]
        public static IntPtr my_func1() 
        {
            // 先にDispose()をする。１つ前の文字列を解放するため。
            hReturnWStringPointer.Dispose();
            hReturnWStringPointer = new StaticWStrPtrHandle("abc");
            return hReturnWStringPointer.DangerousGetHandle();
        }

      //  [DllExport]
        public static IntPtr DllDetachFunc_After_Hm866(IntPtr release_status)
        {
            return (IntPtr)1;
        }


    }
}

