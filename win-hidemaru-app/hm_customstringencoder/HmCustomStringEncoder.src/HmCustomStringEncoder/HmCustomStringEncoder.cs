/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

using Hidemaru;

// エンコードできない文字をUnicodeスカラ値表記(U+XXXX)に置き換えるEncoderFallback
internal class HmEncoderScalarValueFallback : EncoderFallback
{
    // 置き換えられる代替文字列の最大文字数
    public override int MaxCharCount
    {
        get { return 6; /* "U+XXXX".Length */ } // まぁ大丈夫だろ。
    }

    public override EncoderFallbackBuffer CreateFallbackBuffer()
    {
        // EncoderFallbackBufferのインスタンスを作成して返す
        return new HmEncoderScalarValueFallbackBuffer();
    }
}

// 代替文字列の生成を行うクラス
internal class HmEncoderScalarValueFallbackBuffer : EncoderFallbackBuffer
{
    private char[] alternative;
    private int offset;

    // 代替文字列の残り文字数
    public override int Remaining
    {
        get { return alternative.Length - offset; }
    }

    // エンコードできない文字が見つかった場合に呼び出されるメソッド
    public override bool Fallback(char charUnknown, int index)
    {
        // エンコードできない文字をUnicodeスカラ値表記にした文字列を代替文字列とする
        var codes = string.Format("[U+{0:x04}]", (int)charUnknown);

        alternative = codes.ToCharArray();
        var map = HmCustomStringEncoder.map;
        if (map.ContainsKey(codes))
        {
            // すでに有るなら、置き換え
            alternative = map[codes].ToCharArray();
        }

        offset = 0;

        return true;
    }

    public override bool Fallback(char charUnknownHigh, char charUnknownLow, int index)
    {
        // エンコードできない文字をUnicodeスカラ値表記にした文字列を代替文字列とする
        if (!Char.IsHighSurrogate(charUnknownHigh))
        {
            throw new ArgumentOutOfRangeException("charUnknownHigh", "上位文字がサロゲートペア上位文字の範囲外:0xD800, 0xDBFF");
        }
        if (!Char.IsLowSurrogate(charUnknownLow))
        {
            throw new ArgumentOutOfRangeException("charUnknownLow", "下位文字がサロゲートペア上位文字の範囲外:0xDC00, 0xDFFF");
        }

        var codes = String.Format("[U+{0:x04}][U+{1:x04}]", (int)charUnknownHigh, (int)charUnknownLow);
        alternative = codes.ToCharArray();
        var map = HmCustomStringEncoder.map;
        if (map.ContainsKey(codes))
        {
            // すでに有るなら、置き換え
            alternative = map[codes].ToCharArray();
        }


        offset = 0;

        return true;
    }

    // 代替文字列の次の文字を取得するメソッド
    public override char GetNextChar()
    {
        if (alternative.Length <= offset)
        {
            // MaxCharCountやRemainingプロパティの値は考慮されない(?)ようなので、
            // 代替文字列の末尾に到達したらchar.MinValueを返す必要がある
            return char.MinValue;
        }
        else
        {
            return alternative[offset++];
        }
    }

    public override bool MovePrevious()
    {
        return false;
    }

    public override void Reset()
    {
        throw new NotImplementedException();
    }
}


public class HmCustomStringEncoder
{
    // フォールバック時のユーザー定義の対処リスト
    public static Dictionary<string, string> map = new Dictionary<string, string>();


    public static void Open()
    {
        map.Clear();
    }

    public static void PushFallBackPair(String orgcode, String dstcode)
    {
        try
        {
            String codes = "";
            foreach (var ch in orgcode.ToCharArray())
            {
                codes += string.Format("[U+{0:x04}]", (int)ch);
            }

            if (map.ContainsKey(codes))
            {
                // すでに有るなら、置き換え
                map[codes] = dstcode;
            }
            else
            {
                // System.Windows.Forms.MessageBox.Show(codes);
                // 無いなら付け足し
                map.Add(codes, dstcode);
            }
        }
        catch (Exception e)
        {
            System.Diagnostics.Trace.WriteLine(e.Message);
        }
    }

    public static void Close()
    {
        map.Clear();
    }

    public static String ToEncode(IntPtr code, IntPtr nNormalizeForm)
    {
        String text = "";
        try
        {
            if ((Int32)(dynamic)Hm.Macro.Var["selecting"] == 1)
            {
                text = Hm.Edit.SelectedText;
            }
            else
            {
                text = Hm.Edit.TotalText;
            }

            int lineCnt = text.Count(c => c == '\n');
            if (lineCnt < 2)
            {
                text += "\n\n";
            }
        }
        catch (Exception ex0)
        {
            return ex0.GetType() + ":" + ex0.Message + "\n元のファイルにバイナリが混在していないかを確認してください。";
        }


        String normalize_text = "";
        try
        {
            if (nNormalizeForm.ToInt32() == 2)
            {
                normalize_text = text.Normalize();
            }
            else if (nNormalizeForm.ToInt32() == 1)
            {
                StringBuilder sb = new StringBuilder(500 * 1024); // 500k程度をデフォルトとして確保

                //TextElementEnumeratorを作成する
                System.Globalization.TextElementEnumerator tee = System.Globalization.StringInfo.GetTextElementEnumerator(text);
                //読み取る位置をテキストの先頭にする
                tee.Reset();
                //1文字ずつ取得する
                while (tee.MoveNext())
                {
                    //1文字取得する
                    string te = tee.GetTextElement();
                    //1文字が2つ以上のCharから成る場合は、サロゲートペアか結合文字列と判断する
                    if (te.Length > 1)
                    {
                        // ノーマライズして足す
                        sb.Append(te.Normalize());

                        /*
                        //サロゲートペアか調べる
                        if (te.Length == 2 && char.IsSurrogatePair(te, 0))
                        {
                            Console.WriteLine("サロゲートペア「{0}」が「{1}」の位置にあります。",
                                te, tee.ElementIndex);
                        }
                        else
                        {
                            //サロゲートペアでない場合は結合文字列と判断する
                            Console.WriteLine("結合文字列「{0}」が「{1}」の位置にあります。",
                                te, tee.ElementIndex);
                        }
                        */
                    }
                    else
                    {
                        // 普通に足す
                        sb.Append(te);
                    }
                }

                normalize_text = sb.ToString();

            }
            else
            {
                normalize_text = text;
            }
        }
        catch (Exception ex1)
        {
            return ex1.GetType() + ":" + ex1.Message + "\n元のファイルにバイナリが混在していないかを確認してください。";
        }

        try
        {
            // 独自に実装したフォールバックを指定してEncodingを取得
            var encode = Encoding.GetEncoding(code.ToInt32(), new HmEncoderScalarValueFallback(), DecoderFallback.ReplacementFallback);

            var bytes = encode.GetBytes(normalize_text);

            // 再び文字列に戻して
            return encode.GetString(bytes);
        }
        catch (Exception ex2)
        {
            return ex2.GetType() + ":" + ex2.Message + "\n指定の「#ToEncodeCodePage」の値等が正しいか、よく確認してください。";
        }
    }
}