/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */


using System;
using System.Text;
using System.Runtime.InteropServices;



// ★秀丸クラス
internal sealed partial class hmNETDynamicLib
{
    internal sealed partial class Hidemaru
    {
        public sealed class File
        {
            static File()
            {
                SetUnManagedDll();
            }

            static int[] key_encode_value_codepage_array = {
                0,      // Unknown
                932,    // encode = 1 ANSI/OEM Japanese; Japanese (Shift-JIS)
                1200,   // encode = 2 Unicode UTF-16, little-endian
                51932,  // encode = 3 EUC
                50221,  // encode = 4 JIS
                65000,  // encode = 5 UTF-7
                65001,  // encode = 6 UTF-8
                1201,   // encode = 7 Unicode UTF-16, big-endian
                1252,   // encode = 8 欧文 ANSI Latin 1; Western European (Windows)
                936,    // encode = 9 簡体字中国語 ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
                950,    // encode =10 繁体字中国語 ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
                949,    // encode =11 韓国語 ANSI/OEM Korean (Unified Hangul Code)
                1361,   // encode =12 韓国語 Korean (Johab)
                1250,   // encode =13 中央ヨーロッパ言語 ANSI Central European; Central European (Windows)
                1257,   // encode =14 バルト語 ANSI Baltic; Baltic (Windows)
                1253,   // encode =15 ギリシャ語 ANSI Greek; Greek (Windows)
                1251,   // encode =16 キリル言語 ANSI Cyrillic; Cyrillic (Windows)
                42,     // encode =17 シンボル
                1254,   // encode =18 トルコ語 ANSI Turkish; Turkish (Windows)
                1255,   // encode =19 ヘブライ語 ANSI Hebrew; Hebrew (Windows)
                1256,   // encode =20 アラビア語 ANSI Arabic; Arabic (Windows)
                874,    // encode =21 タイ語 ANSI/OEM Thai (same as 28605, ISO 8859-15); Thai (Windows)
                1258,   // encode =22 ベトナム語 ANSI/OEM Vietnamese; Vietnamese (Windows)
                10001,  // encode =23 x-mac-japanese Japanese (Mac)
                850,    // encode =24 OEM/DOS
                0,      // encode =25 その他
                12000,  // encode =26 Unicode (UTF-32) little-endian
                12001,  // encode =27 Unicode (UTF-32) big-endian

            };

            public static int GetHmEncode(string filename)
            {
                if (version < 890)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return -1;
                }

                if (pAnalyzeEncoding == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return -1;
                }

                return pAnalyzeEncoding(filename, IntPtr.Zero, IntPtr.Zero);
            }

            // columnやlinenoはエディタ的な座標である。
            public static int GetMsCodePage(string filename)
            {

                int result_codepage = 0;

                if (version < 890)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return result_codepage;
                }

                if (pAnalyzeEncoding == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return result_codepage;
                }


                int hidemaru_encode = pAnalyzeEncoding(filename, IntPtr.Zero, IntPtr.Zero);

                /*
                 *
                    Shift-JIS encode=1 codepage=932
                    Unicode encode=2 codepage=1200
                    EUC encode=3 codepage=51932
                    JIS encode=4 codepage=50221
                    UTF-7 encode=5 codepage=65000
                    UTF-8 encode=6 codepage=65001
                    Unicode (Big-Endian) encode=7 codepage=1201
                    欧文 encode=8 codepage=1252
                    簡体字中国語 encode=9 codepage=936
                    繁体字中国語 encode=10 codepage=950
                    韓国語 encode=11 codepage=949
                    韓国語(Johab) encode=12 codepage=1361
                    中央ヨーロッパ言語 encode=13 codepage=1250
                    バルト語 encode=14 codepage=1257
                    ギリシャ語 encode=15 codepage=1253
                    キリル言語 encode=16 codepage=1251
                    シンボル encode=17 codepage=42
                    トルコ語 encode=18 codepage=1254
                    ヘブライ語 encode=19 codepage=1255
                    アラビア語 encode=20 codepage=1256
                    タイ語 encode=21 codepage=874
                    ベトナム語 encode=22 codepage=1258
                    Macintosh encode=23 codepage=0
                    OEM/DOS encode=24 codepage=0
                    その他 encode=25 codepage=0
                    UTF-32 encode=27 codepage=12000
                    UTF-32 (Big-Endian) encode=28 codepage=12001
                */
                if (hidemaru_encode <= 0)
                {
                    return result_codepage;
                }

                if (hidemaru_encode < key_encode_value_codepage_array.Length)
                {
                    // 把握しているコードページなので入れておく
                    result_codepage = key_encode_value_codepage_array[hidemaru_encode];
                    return result_codepage;
                }
                else // 長さ以上なら、予期せぬ未来のencode番号対応
                {
                    return result_codepage;
                }
            }
        }
    }
}

