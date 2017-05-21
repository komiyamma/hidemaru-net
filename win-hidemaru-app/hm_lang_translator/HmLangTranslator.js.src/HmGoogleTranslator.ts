///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>


/**
 * Google用に導出されたクラス
 */
class GoogleTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {

    protected InitializeQueryParams(): void {
        // 翻訳対象の元テキスト。
        this.queryParams.Add("text", this.SrcText);

        // 何語から何語へ翻訳するのか
        this.queryParams.Add("langpair", `${this.targetLanguages.src}|${this.targetLanguages.dst}`);

        // 結果ページで使用される言語
        // (翻訳内容とは無関係で、GUIインターフェイスの言語。日本語で良い)
        this.queryParams.Add("hl", "ja");

        // 入力文字列の文字コード
        this.queryParams.Add("ie", "UTF8");

        // 結果文字列の文字コード
        this.queryParams.Add("oe", "UTF8");
    }

    get Method(): QueryMethodType {
        return "GET";
    }

    get Url(): string {
        return "http://translate.google.com/translate_t";
    }

    /**
     * 結果のページから対象の部分を抽出するどろ臭い部分
     * Googleの場合は、HTML。
     * @param result_page
     */
    FilterResultText(result_page: string): string {

        // id=result_boxあたりのタグ内のInnerTextが翻訳語の文字列。
        // 本格的にやるなら「HtmlAgilityPack」で、対象のidタグのInnerTextを求めるのが良いが大仰である。
        // 正規表現で対処してしまって良いだろう。
        // 複数行考慮
        let resultAroundTextRegexp: RegExp = /<span id=result_box [\s\S]+?<\/span><\/span>/;

        // 結果のHTMLページ全体から、翻訳結果周辺をまずは抽出。
        let resultAroundResultArray: RegExpExecArray | null = resultAroundTextRegexp.exec(result_page);

        // 存在する
        if (resultAroundResultArray) {
            // hm.Edit.TotalText = resultAroundResultArray[0];
            // 得た「周辺テキスト」にたいして…
            let regexpResultText: string = resultAroundResultArray[0];
            // hm.Edit.TotalText = regexpResultText;
            // さらにタグを全部除去。複数行考慮
            regexpResultText = regexpResultText.replace(/<("[^"]*"|'[^']*'|[^'">])*>/g, "");

            // 結果文字列がHTMLエンコードされてるのでHTMLデコードする。
            // (ただし元々がHTMLエンコードした状態の文字列だと区別が付かないという不正確さがある)
            regexpResultText = this.HttpUtility.HtmlDecode(regexpResultText);

            return regexpResultText;

        } else {
            return "";
        }
    }
}


// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "en", dst: "ja" };
// let result_words: string = ContextTranslator.Translate(GoogleTranslatorQueryStrategy, langParams);