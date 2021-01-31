using System;
using System.Runtime.ConstrainedExecution;
using System.Runtime.InteropServices;
using System.Security.Permissions;





namespace Pattern2
{

    public class Pattern2
    {

        [DllExport]
        public static IntPtr MyFunc1(IntPtr a, IntPtr b)
        {
            return a;
        }

        [DllExport]
        public unsafe static IntPtr MyFunc2(IntPtr obj)
        {
            System.Diagnostics.Trace.WriteLine(obj.ToString());
            System.Diagnostics.Trace.WriteLine(obj.GetType().ToString());

            // 文字列を指す生ポインタの内容をマネージド文字列へとコピー
            // String managed_str = new string(p_str);

            // 文字列の長さを返す。
            return (IntPtr)1; //(IntPtr)managed_str.Length;
        }

        [DllExport]
        public unsafe static IntPtr MyFunc3(char* p_wstr)
        {
            String wstr = new String(p_wstr);
            System.Diagnostics.Trace.WriteLine(wstr);
            // 文字列の長さを返す。
            return (IntPtr)(wstr.Length);
        }


        // [return: MarshalAs(UnmanagedType.LPWStr)]
        static String rtnBufferOfGetString = "";
        [DllExport]
        public unsafe static char* GetString()
        {
            rtnBufferOfGetString = "333";
            // マネージドな文字列の内容を指す生ポインタを返す。
            // 生ポインタを指す内容が関数の終了とともに消えないことを保証するためには、rtnStringBuffer はメソッドスコープではなくクラススコープであるべきだ。
            // 生ポインタが指す文字列の中身は秀丸はいつまでも要求するわけではなく、このメソッド呼び出し時に秀丸マクロ層で変数で受け止めていれば、
            // 秀丸側は「文字列をコピーする」。
            // よって、「メソッドが呼び出された瞬間だけ生ポインタは文字列をさしていればよく」いつまでも文字列を保持する必要はない。
            fixed (char* p_str = rtnBufferOfGetString) { return p_str; }
            // return strBufferOfGetString;
        }


        [DllExport]
        public unsafe static IntPtr DllDetachFunc_After_Hm866(IntPtr release_status)
        {
            System.Diagnostics.Trace.WriteLine(release_status);
            return (IntPtr)1;
        }
    }
}
