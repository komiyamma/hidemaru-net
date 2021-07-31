/*
 * Copyright (c) 2021 Akitsugu Komiyama
 * under the MIT License
 */


using System;
using System.Collections.Generic;



// ★秀丸クラス
public sealed partial class hmPSDynamicLib
{
    public sealed partial class Hidemaru
    {

        public static TOutputPane OutputPane;
        public sealed class TOutputPane
            {
            public TOutputPane()
            {
                SetUnManagedDll();
            }


            // Output枠へと出力する
            public static int Output(string message)
            {
                try
                {
                    if (pOutputPane_OutputW != null)
                    {
                        int result = pOutputPane_OutputW(Hidemaru.WindowHandle, message);
                        return result;
                    }
                    else
                    {
                        byte[] encode_data = HmOriginalEncodeFunc.EncodeWStringToOriginalEncodeVector(message);
                        int result = pOutputPane_Output(Hidemaru.WindowHandle, encode_data);
                        return result;
                    }
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
                return SendMessage(1009);

            }

            public static IntPtr WindowHandle
            {
                get
                {
                    return pOutputPane_GetWindowHandle(Hidemaru.WindowHandle);
                }
            }

            public static int SendMessage(int commandID)
            {
                //
                // loaddll "HmOutputPane.dll";
                // #h=dllfunc("GetWindowHandle",hidemaruhandle(0));
                // #ret=sendmessage(#h,0x111,1009,0);//1009=クリア 0x111=WM_COMMAND
                //
                IntPtr r = hmPSDynamicLib.SendMessage(pOutputPane_GetWindowHandle(Hidemaru.WindowHandle), 0x111, commandID, IntPtr.Zero);
                if ((long)r < (long)int.MinValue)
                {
                    r = (IntPtr)int.MinValue;
                }
                if ((long)r > (long)int.MaxValue)
                {
                    r = (IntPtr)int.MaxValue;
                }
                return (int)r;
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

