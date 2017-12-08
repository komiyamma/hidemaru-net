/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Text.RegularExpressions;

public class HmJapanesePhoneticAnalyzer
{
    string m_text = "";
    int m_char_ix = 0;

    public HmJapanesePhoneticAnalyzer() { }

    public List<JapanesePhoneticAnalyzeData> GetJapanesePhoneticAnalyzeDataList(String text)
    {
        this.m_char_ix = 0;
        this.m_text = text;
        japanesePhoneticAnalyzeDataList.Clear();

        var list = Windows.Globalization.JapanesePhoneticAnalyzer.GetWords(text, false); // ２番目のmonorubyをtrueにすると、全て１文字ずつルビを打ってしまう。
        foreach (var data in list)
        {
            var japanesePhoneticAnalyzeData = new JapanesePhoneticAnalyzeData();

            // 読みはそのまま入れる
            japanesePhoneticAnalyzeData.YomiText = data.YomiText;

            // 元の文字のどこからどこまでをコピーすればいいのかを勘定
            // 元の文字列を使う必要性がある理由は、Windows.Globalization.JapanesePhoneticAnalyzerが自動で文字列を全角にしてしまうための対処
            var len = data.DisplayText.Length;
            var start = m_char_ix;
            m_char_ix = m_char_ix + len;
            japanesePhoneticAnalyzeData.DisplayText = m_text.Substring(start, len);

            // 
            japanesePhoneticAnalyzeData.IsPhraseStart = IsContainsKanji(japanesePhoneticAnalyzeData.DisplayText);
            japanesePhoneticAnalyzeDataList.Add(japanesePhoneticAnalyzeData);
        }

        // ExecuteAnalyze(m_text);
        return japanesePhoneticAnalyzeDataList;
    }

    public class JapanesePhoneticAnalyzeData
    {
        public string DisplayText;
        public bool IsPhraseStart;
        public string YomiText;
    }

    List<JapanesePhoneticAnalyzeData> japanesePhoneticAnalyzeDataList = new List<JapanesePhoneticAnalyzeData>();

    bool IsContainsKanji(string text)
    {
        //漢字が含まれているか調べる
        if (Regex.IsMatch(text,
            @"[\p{IsCJKUnifiedIdeographs}" +
            @"\p{IsCJKCompatibilityIdeographs}" +
            @"\p{IsCJKUnifiedIdeographsExtensionA}]|" +
            @"[\uD840-\uD869][\uDC00-\uDFFF]|\uD869[\uDC00-\uDEDF]"))
        {
            return true;
        }
        return false;
    }

}