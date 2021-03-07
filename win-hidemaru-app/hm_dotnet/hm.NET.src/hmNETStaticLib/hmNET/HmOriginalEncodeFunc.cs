using System;
using System.Collections.Generic;

internal sealed partial class hmNETDynamicLib
{
    internal sealed partial class Hidemaru
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

            /*
            // 元へと戻す際に構築する逆引き辞書。ほとんどのシーンでは利用されないであろうから、必要となった時はじめて逆引き辞書を作る。
            private static Dictionary<UInt32, UInt16> sorted_map = null;
            private static void InitReverseDictionary()
            {
                if (sorted_map == null)
                {
                    sorted_map = new Dictionary<UInt32, UInt16>();
                    for (int i = 0; i < HmOriginalEncodeMap.encode_map.Length; i++)
                    {
                        UInt32 key = HmOriginalEncodeMap.encode_map[i];
                        sorted_map.Add(key, (UInt16)i);
                    }
                }
            }

            // 秀丸独自エンコード→wchar_tへ。
            // wchar_tに直接対応していないような古い秀丸では、この特殊な変換マップによる変換をしてバイトコードから元のwchar_tへと戻す必要がある。
            public static string DecodeOriginalEnncodeVectorToWString(byte[] encode_data)
            {
                InitReverseDictionary();

                int index = 0;
                string wstr = "";

                while (index < encode_data.Length)
                {
                    bool found = false;
                    if (encode_data.Length - index > 4)
                    {
                        byte[] buf4 = new byte[] { encode_data[index + 0], encode_data[index + 1], encode_data[index + 2], encode_data[index + 3] };
                        UInt32 map_key4 = BitConverter.ToUInt32(buf4, 0);
                        if (sorted_map.ContainsKey(map_key4))
                        {
                            wstr += (char)(sorted_map[map_key4]);
                            index += 4;
                            found = true;
                        }
                    }

                    if (!found)
                    {
                        if (encode_data.Length - index > 3)
                        {
                            byte[] buf3 = new byte[] { encode_data[index + 0], encode_data[index + 1], encode_data[index + 2], 0 };
                            UInt32 map_key3 = BitConverter.ToUInt32(buf3, 0);
                            if (sorted_map.ContainsKey(map_key3))
                            {
                                wstr += (char)(sorted_map[map_key3]);
                                index += 3;
                                found = true;
                            }
                        }
                    }

                    if (!found)
                    {
                        if (encode_data.Length - index > 2)
                        {
                            byte[] buf2 = new byte[] { encode_data[index + 0], encode_data[index + 1], 0, 0 };
                            UInt32 map_key2 = BitConverter.ToUInt32(buf2, 0);
                            if (sorted_map.ContainsKey(map_key2))
                            {
                                UInt16 a = (UInt16)'あ';
                                UInt16 value = sorted_map[map_key2];
                                wstr += (char)(value);
                                index += 2;
                                found = true;
                            }
                        }
                    }

                    if (!found)
                    {
                        if (encode_data.Length - index > 1)
                        {
                            byte[] buf1 = new byte[] { encode_data[index + 0], 0, 0, 0 };
                            UInt32 map_key1 = BitConverter.ToUInt32(buf1, 0);
                            if (map_key1 != 0 && sorted_map.ContainsKey(map_key1))
                            {
                                wstr += (char)(sorted_map[map_key1]);
                                index += 1;
                                found = true;
                            }
                        }
                    }

                    if (!found)
                    {
                        char ch = (char)encode_data[index + 0];
                        if (ch != 0)
                        {
                            wstr += ch;
                        }
                        index += 1;
                    }
                }
                return wstr;
            }
            */
        }
    }
}
