/*
 * Copyright (c) 2021 Akitsugu Komiyama
 * under the MIT License
 */


using System;
using System.Collections.Generic;



// ★秀丸クラス
public sealed partial class hmJSDynamicLib
{
    public sealed partial class Hidemaru
    {
        public sealed class OutputPane
        {
            static OutputPane()
            {
                SetUnManagedDll();
            }


            // Output枠へと出力する
            public static int Output(object message)
            {
                try
                {
                    string str_message = message.ToString();
                    byte[] encode_data = HmOriginalEncodeFunc.EncodeWStringToOriginalEncodeVector(str_message);
                    int result = pOutputPane_Output(Hidemaru.WindowHandle, encode_data);
                    return result;
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedOutputOperation + ":\n" + e.Message);
                }

                return 0;
            }

            // Output枠でのPush
            public static int Push()
            {
                return pOutputPane_Push(Hidemaru.WindowHandle); ;
            }

            // Output枠でのPush
            public static int Pop()
            {
                return pOutputPane_Pop(Hidemaru.WindowHandle); ;
            }

            public static int Clear()
            {
                //1009=クリア
                IntPtr r = OutputPane.SendMessage(1009);
                if ((long)r < (long)int.MinValue)
                {
                    r = (IntPtr)int.MinValue;
                }
                if ((long)int.MaxValue < (long)r)
                {
                    r = (IntPtr)int.MaxValue;
                }

                return (int)r;

            }

            public static IntPtr WindowHandle
            {
                get
                {
                    return pOutputPane_GetWindowHandle(Hidemaru.WindowHandle);
                }
            }

            public static IntPtr SendMessage(int commandID)
            {
                //
                // loaddll "HmOutputPane.dll";
                // #h=dllfunc("GetWindowHandle",hidemaruhandle(0));
                // #ret=sendmessage(#h,0x111,1009,0);//1009=クリア 0x111=WM_COMMAND
                //
                IntPtr result = hmJSDynamicLib.SendMessage(OutputPane.WindowHandle, 0x111, commandID, IntPtr.Zero);
                return result;
            }

            // Output枠へと出力する
            public static int SetBaseDir(string dirpath)
            {
                if (version < 877)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed877);
                    return 0;
                }

                try
                {
                    if (pOutputPane_SetBaseDir != null)
                    {
                        byte[] encode_data = HmOriginalEncodeFunc.EncodeWStringToOriginalEncodeVector(dirpath);
                        int result = pOutputPane_SetBaseDir(Hidemaru.WindowHandle, encode_data);
                        return result;
                    }
                }
                catch (Exception e)
                {
                    OutputDebugStream(ErrorMsg.MethodNeedOutputOperation + ":\n" + e.Message);
                }

                return 0;
            }

        }
    }
}

