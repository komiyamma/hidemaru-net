/*
 * Copyright (c) 2017 Akitsugu Komiyama
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

                foreach(char ch in original_string) {
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
                        r.AddRange (bytes4);
                    }
                }
                r.Add(0);
                return r.ToArray();
            }

            // ファイルを開いて情報を得る
            public static int Output(string message)
            {
                try {
                    byte[] encode_data = EncodeWStringToOriginalEncodeVector(message);
                    int result = pOutputPane_Output(WindowHandle, encode_data);
                    return result;
                } catch(Exception e) {
                    OutputDebugStream(ErrorMsg.MethodNeedOutputOperation + ":\n" + e.Message);
                }

                return 0;
            }

        }
    }
}

