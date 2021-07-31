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
                bool isClearScriptItem = false;
                try
                {
                    if (message.GetType().Name == "WindowsScriptItem" || message.GetType().BaseType?.Name == "WindowsScriptItem" || message.GetType().BaseType?.BaseType?.Name == "WindowsScriptItem")
                    {
                        isClearScriptItem = true;
                    }
                }
                catch (Exception )
                {

                }

                string str_message = "";
                // ClearScriptエンジンのオブジェクトであれば、そのまま出しても意味が無いので…
                if (isClearScriptItem)
                {
                    dynamic dexp = message;

                    // JSON.stringifyで変換できるはずだ
                    try
                    {
                        str_message = engine.Script.JSON.stringify(dexp);
                    }
                    catch (Exception)
                    {

                    }

                    // JSON.stringfyで空っぽだったようだ。
                    if (str_message == String.Empty)
                    {
                        try
                        {
                            // ECMAScriptのtoString()で変換出来るはずだ…
                            str_message = dexp.toString();
                        }
                        catch (Exception)
                        {
                            // 変換できなかったら、とりあえず、しょうがないので.NETのToStringで。多分意味はない。
                            str_message = message.ToString();
                        }
                    }
                }

                // WindowsScriptItemオブジェクトでないなら、普通にToString
                else
                {
                    str_message = message.ToString();
                }

                try
                {
                    if (pOutputPane_OutputW != null)
                    {
                        int result = pOutputPane_OutputW(Hidemaru.WindowHandle, str_message);
                        return result;
                    }
                    else
                    {
                        byte[] encode_data = HmOriginalEncodeFunc.EncodeWStringToOriginalEncodeVector(str_message);
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

