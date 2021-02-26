using System;
using System.Collections.Generic;

public sealed partial class hmRbDynamicLib
{
    public sealed partial class Hidemaru
    {
        internal class HmOriginalEncodeFunc
        {
            // wchar_tに直接対応していないような古い秀丸では、この特殊な変換マップによる変換をしてバイトコードとして渡す必要がある。
            public static byte[] EncodeWStringToOriginalEncodeVector(string original_string)
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
        }
    }
}
