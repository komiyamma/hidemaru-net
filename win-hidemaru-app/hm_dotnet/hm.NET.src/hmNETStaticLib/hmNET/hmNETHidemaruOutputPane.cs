/*
 * Copyright (c) 2021 Akitsugu Komiyama
 * under the MIT License
 */


using System;
using System.Text;
using System.Runtime.InteropServices;
using Hidemaru;
using System.Collections.Generic;



// ★秀丸クラス
internal sealed partial class hmNETDynamicLib
{
    internal sealed partial class Hidemaru
    {
        public sealed class OutputPane
        {
            static OutputPane()
            {
                SetUnManagedDll();
            }

            // wchar_tに直接対応していないような古い秀丸では、この特殊な変換マップによる変換をしてバイトコードとして渡す必要がある。
            private static byte[] EncodeWStringToOriginalEncodeVector(string original_string)
            {
                List<Byte> r = new List<byte>();

                foreach (char ch in original_string)
                {
                    // 文字コードがそんまま、マップでのIndexになっている。
                    int ix = (int)ch;

                    // 文字コードに対応する秀丸での4バイトの値(UInt32)を得る4バイト
                    UInt32 encode_code = HmOriginalEncodeMap.encode_map[ix];

                    // 単バイトで収まるものは単バイト
                    if (encode_code <= 0xFF)
                    { // １文字で収まる
                        r.Add((Byte)encode_code);
                    }
                    // そうでないもののは４バイト、それぞれ分割で追加
                    else
                    {
                        byte[] bytes4 = BitConverter.GetBytes(encode_code);
                        foreach (byte b in bytes4)
                        {
                            if (b == 0) { break; }
                            r.Add(b);
                        }
                    }
                }

                r.Add(0);
                return r.ToArray();
            }

            // Output枠へと出力する
            public static int Output(string message)
            {
                try
                {
                    byte[] encode_data = EncodeWStringToOriginalEncodeVector(message);
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

            public static IntPtr WindowHandle
            {
                get
                {
                    return pGetWinndowHandle(Hidemaru.WindowHandle);
                }
            }

            public static IntPtr SendMessge(int commandID)
            {
                //
                // loaddll "HmOutputPane.dll";
                // #h=dllfunc("GetWindowHandle",hidemaruhandle(0));
                // #ret=sendmessage(#h,0x111,1009,0);//1009=クリア 0x111=WM_COMMAND
                //
                IntPtr result = SendMessage(OutputPane.WindowHandle, 0x111, commandID, IntPtr.Zero);
                return result;
            }
        }
    }
}

