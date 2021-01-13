/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */


using System;
using System.Runtime.InteropServices;



// ★秀丸クラス
public sealed partial class hmRbDynamicLib
{
    public sealed partial class Hidemaru
    {
        public HmFile File = new HmFile();
        public sealed class HmFile
        {
            public HmFile()
            {
                SetUnManagedDll();
            }

            public interface IHidemaruEncoding
            {
                int HmEncode { get; }
            }
            public interface IMicrosoftEncoding
            {
                int MsCodePage { get; }
            }

            public interface IEncoding : IHidemaruEncoding, IMicrosoftEncoding
            {
            }

            // 途中でエラーが出るかもしれないので、相応しいUnlockやFreeが出来るように内部管理用
            private enum HGlobalStatus { None, Lock, Unlock, Free };

            // IronRuby で外から見る場合、static であってはならない。
            public String ReadAllText(String filepath, int hm_encode = -1)
            {
                return _ReadAllText(filepath, hm_encode);
            }
            private static String _ReadAllText(String filepath, int hm_encode = -1)
            {
                if (_ver < 890)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);
                    return "";
                }
                if (pLoadFileUnicode == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return "";
                }

                if (hm_encode == -1)
                {
                    hm_encode = GetHmEncode(filepath);
                }

                if (!System.IO.File.Exists(filepath))
                {
                    throw new System.IO.FileNotFoundException(filepath);
                }

                String curstr = "";
                int read_count = 0;
                IntPtr hGlobal = pLoadFileUnicode(filepath, hm_encode, ref read_count, IntPtr.Zero, IntPtr.Zero);
                HGlobalStatus hgs = HGlobalStatus.None;
                if (hGlobal == IntPtr.Zero)
                {
                    throw new System.IO.IOException(filepath);
                }
                if (hGlobal != IntPtr.Zero)
                {
                    try
                    {
                        IntPtr ret = GlobalLock(hGlobal);
                        hgs = HGlobalStatus.Lock;
                        curstr = Marshal.PtrToStringUni(ret);
                        GlobalUnlock(hGlobal);
                        hgs = HGlobalStatus.Unlock;
                        GlobalFree(hGlobal);
                        hgs = HGlobalStatus.Free;
                    }
                    catch (Exception e)
                    {
                        OutputDebugStream(e.Message);
                    }
                    finally
                    {
                        switch (hgs)
                        {
                            // ロックだけ成功した
                            case HGlobalStatus.Lock:
                                {
                                    GlobalUnlock(hGlobal);
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // アンロックまで成功した
                            case HGlobalStatus.Unlock:
                                {
                                    GlobalFree(hGlobal);
                                    break;
                                }
                            // フリーまで成功した
                            case HGlobalStatus.Free:
                                {
                                    break;
                                }
                        }
                    }
                }
                if (hgs == HGlobalStatus.Free)
                {
                    return curstr;
                }
                else
                {
                    throw new System.IO.IOException(filepath);
                }
            }

            private static int[] key_encode_value_codepage_array = {
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

            private class Encoding : IEncoding
            {
                private int m_hm_encode;
                private int m_ms_codepage;
                public Encoding(int hmencode, int mscodepage)
                {
                    this.m_hm_encode = hmencode;
                    this.m_ms_codepage = mscodepage;
                }
                public int HmEncode { get { return this.m_hm_encode; } }
                public int MsCodePage { get { return this.m_ms_codepage; } }
            }

            // ★ IronRuby で外から見えるやつは、static であってはならない。
            // Encoding系情報を得る
            public IEncoding GetEncoding(string filepath)
            {
                return _GetEncoding(filepath);
            }

            private static IEncoding _GetEncoding(string filepath)
            {
                int hm_encode = GetHmEncode(filepath);
                int ms_codepage = GetMsCodePage(hm_encode);
                IEncoding encoding = new Hidemaru.HmFile.Encoding(hm_encode, ms_codepage);
                return encoding;
            }

            private static int GetHmEncode(string filepath)
            {
                if (_ver < 890)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return -1;
                }

                if (pAnalyzeEncoding == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return -1;
                }

                return pAnalyzeEncoding(filepath, IntPtr.Zero, IntPtr.Zero);
            }

            private static int GetMsCodePage(int hidemaru_encode)
            {
                int result_codepage = 0;

                if (_ver < 890)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return result_codepage;
                }

                if (pAnalyzeEncoding == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return result_codepage;
                }

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

            // コードページを得る
            private static int GetMsCodePage(string filepath)
            {

                int result_codepage = 0;

                if (_ver < 890)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return result_codepage;
                }

                if (pAnalyzeEncoding == null)
                {
                    OutputDebugStream(ErrorMsg.MethodNeed890);

                    return result_codepage;
                }


                int hidemaru_encode = pAnalyzeEncoding(filepath, IntPtr.Zero, IntPtr.Zero);

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


