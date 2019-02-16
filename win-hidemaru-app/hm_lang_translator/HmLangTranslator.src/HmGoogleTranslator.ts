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
    InitializeQueryParams(): void {
        // 翻訳対象の元テキスト。
        this.QueryParams.Add("text", this.SrcText);
        // 何語から何語へ翻訳するのか
        this.QueryParams.Add("source", this.TargetLanguages.src);
        this.QueryParams.Add("target", this.TargetLanguages.dst);
    }
    get Method(): QueryMethodType {
        return "GET"; // GETは動作するがPOSTはGoogle側が受け付けなくなった？
    }
    get Url(): string {
        return "https://script.google.com/macros/s/**********************************************/exec";
    }
    /**
     * 結果のページから対象の部分を抽出するどろ臭い部分
     * Googleの場合は、HTML。
     * @param result_page
     */
    FilterResultText(result_page: string): string {
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

