///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmGoogleTranslator v1.03
 * Copyright (C) 2017-2019 Akitsugu Komiyama
 * under the MIT License
 */



/**
 * Google用に導出されたクラス
 */
class GoogleTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {
    InitializeQueryParams() {
        // 翻訳対象の元テキスト。
        this.QueryParams.Add("text", this.SrcText);
        // 何語から何語へ翻訳するのか
        this.QueryParams.Add("source", this.TargetLanguages.src);
        this.QueryParams.Add("target", this.TargetLanguages.dst);
    }
    get Method() {
        return "POST"; // 動作を簡単にするためGET
    }
    get Url() {
        // Google Apps Script で自分で得たURLを取得
        return "https://script.google.com/macros/s/**********************************************/exec";
    }
    /**
     * 結果のページから対象の部分を抽出するどろ臭い部分
     * Googleの場合は、HTML。
     * @param result_page
     */
    FilterResultText(result_page) {
        if (result_page) {
            return result_page;
        }
        else {
            return "";
        }
    }
}
// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "en", dst: "ja" };
// let result_words: string = ContextTranslator.Translate(GoogleTranslatorQueryStrategy, langParams); 
