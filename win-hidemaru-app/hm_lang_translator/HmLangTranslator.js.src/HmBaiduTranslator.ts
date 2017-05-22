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

    protected InitializeQueryParams(): void {
        // http://fanyi.baidu.com/v2transapi?query=apple&from=en&to=jp

        // Baiduのクエリ上での改行は\r(=0xA)である必要がある。
        let srcText: string = this.SrcText;
        srcText = srcText.replace(/\r?\n/, "\r");

        // 翻訳対象の元テキスト。
        this.QueryParams.Add("query", srcText);

        // 翻訳元の言語
        this.QueryParams.Add("from", this.TargetLanguages.src);

        // 翻訳先の言語
        this.QueryParams.Add("to", this.TargetLanguages.dst);
    }

    get Method(): QueryMethodType {
        return "POST"; // GETでもPOSTでも両方動作する。
    }

    get Url(): string {
        return "http://fanyi.baidu.com/v2transapi";
    }

    /**
     * 結果のページはJsonによって格納されてる。
     * @param result_page
     */
    FilterResultText(result_page: string): string {

        let res: any = JSON.parse(result_page);
        if (res.error) {
            return "";
        } else {
            return res.trans_result.data[0].dst;
        }
    }
}

// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "en", dst: "jp" };
// let result_words: string = ContextTranslator.Translate(BaiduTranslatorQueryStrategy, langParams);