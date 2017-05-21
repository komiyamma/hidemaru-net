///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmBaiduTranslator v1.01
 * Copyright (C) @ 2017 VSCode.life
 */
/**
 * Baidu用に導出されたクラス
 */
class BaiduTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {
    InitializeQueryParams() {
        // http://fanyi.baidu.com/v2transapi?query=apple&from=en&to=jp
        // 翻訳対象の元テキスト。
        this.QueryParams.Add("query", this.SrcText);
        // 翻訳元の言語
        this.QueryParams.Add("from", this.TargetLanguages.src);
        // 翻訳先の言語
        this.QueryParams.Add("to", this.TargetLanguages.dst);
    }
    get Method() {
        return "GET";
    }
    get Url() {
        return "http://fanyi.baidu.com/v2transapi";
    }
    /**
     * 結果のページはJsonによって格納されてる。
     * @param result_page
     */
    FilterResultText(result_page) {
        let res = JSON.parse(result_page);
        if (res.error) {
            return "";
        }
        else {
            return res.trans_result.data[0].dst;
        }
    }
}
// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "en", dst: "jp" };
// let result_words: string = ContextTranslator.Translate(BaiduTranslatorQueryStrategy, langParams); 
