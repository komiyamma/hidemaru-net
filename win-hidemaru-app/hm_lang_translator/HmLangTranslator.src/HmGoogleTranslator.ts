///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmGoogleTranslator v1.02
 * Copyright (C) 2017 VSCode.life
 */

/**
 * Google用に導出されたクラス
 */
class GoogleTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {

    protected InitializeQueryParams(): void {
        // 翻訳対象の元テキスト。
        this.QueryParams.Add("text", this.SrcText);

        // これが必要。これがあれば、答えのテキストだけで返ってくる。
        this.QueryParams.Add("client", "j");

        // 何語から(source language)
        this.QueryParams.Add("sl", <string>this.TargetLanguages.src);

        // 何語へ(target language)
        this.QueryParams.Add("tl", <string>this.TargetLanguages.dst);

        // 結果ページで使用される言語
        // (翻訳内容とは無関係で、GUIインターフェイスの言語。日本語で良い)
        this.QueryParams.Add("hl", "ja");

        // 入力文字列の文字コード
        this.QueryParams.Add("ie", "UTF8");

        // 結果文字列の文字コード
        this.QueryParams.Add("oe", "UTF8");
    }

    get Method(): QueryMethodType {
        return "GET"; // GETは動作するがPOSTはGoogle側が受け付けなくなった？
    }

    get Url(): string {
        return "http://translate.google.com/translate_a/t";
    }

    /**
     *  @param result_page: 結果のページはJavaScriptの文字列フォーマット、
     *  もしくはJavaScriptの文字列の配列によって格納されてる。
     *  "りんご"
     *    という形かもしくは
     *  ["りんご", "en"]
     *    という形。
     *  ２番めの要素には、翻訳元の言語をautoで自動判断させた際に、何語と自動判断されたのか、が入る。
     */
    FilterResultText(result_page: string): string {

        let page_text: string = result_page;

        // 具体的に示されている時には、jsonの体をしていないので、[ ] でjson的な形にしてしまう。
        if (!result_page.startsWith("[")) {
            page_text = "[" + page_text + "]";
        }

        // フォーマットが本当に合っているのか裏付けが薄いのでtry...catchしておく。
        try {
            let res: any = JSON.parse(page_text);
            return res[0];
        } catch(err) {
            PrintOutputPane(err);
        }
        return "";
    }
}


// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "en", dst: "ja" };
// let result_words: string = ContextTranslator.Translate(GoogleTranslatorQueryStrategy, langParams);