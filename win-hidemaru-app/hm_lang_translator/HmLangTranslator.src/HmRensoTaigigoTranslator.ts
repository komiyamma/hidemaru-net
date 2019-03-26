///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmRensoTaigigoTranslator v1.03
 * Copyright (C) 2017-2019 Akitsugu Komiyama
 * under the MIT License
 */

interface IRensoTaigigoElement {
    word: string; // 連想対義語
}

class IterableRensoTaigigoElementList {
    m_resultStructArray: IRensoTaigigoElement[];

    constructor(result_page: string) {
        this.m_resultStructArray = this.ConvertStringToStructArray(result_page);
    }

    /**
     * づらずらと未フォーマットな文字列になっているので構造を把握しやすい配列へと格納する。
     * @param result_page 
     */
    protected ConvertStringToStructArray(result_page: string): IRensoTaigigoElement[] {
        // 途中の改行は面倒くさいので除去
        let simgleResultPage: string = result_page.replace(/[\r\n]/, "");

        // word_t_fieldのタグ内が答えのリスト一覧
        let resultAroundTextRegexp: RegExp = /<div class=Wtght>([\s\S]+?)<\/div><\/div>/;
        let resultAroundExecArray: RegExpExecArray | null = resultAroundTextRegexp.exec(simgleResultPage);

        // １つも無い
        if (!resultAroundExecArray) {
            return [];
        }

        let joinedRensoWordText: string = resultAroundExecArray[0];
        console.log(joinedRensoWordText);

        let resultStructArray: IRensoTaigigoElement[] = [];
        // 結果パターンの繰り返し
        let resultTextRegexp: RegExp = /<td class=wtghtTW><span class=wtghtAntnm>(.+?)<\/span>[\s\S]+?<br class=clr>/g;
        let resultExecArray: RegExpExecArray | null;
        // １つずつ抽出して、IRensoTaigigoElement型にして IRensoTaigigoElement配列へと足し込み
        while (resultExecArray = resultTextRegexp.exec(joinedRensoWordText)) {

            let word = resultExecArray[1];
            // さらにタグを全部除去。複数行考慮
            word = word.replace(/<("[^"]*"|'[^']*'|[^'">])*>/g, "");

            let element: IRensoTaigigoElement = { word: word };
            resultStructArray.push(element);
        }

        return resultStructArray;
    }

    /**
     * イテレータ。
     */
    *[Symbol.iterator](): IterableIterator<IRensoTaigigoElement> {
        for (let e of this.m_resultStructArray) {
            yield e;
        }
    }

    /**
     * 内部的に綺麗に構造化して管理している内容を１つの整形された連結テキストにする。
     */
    formattedString(): string {
        return this.ConvertStructArrayToFormattedString(this.m_resultStructArray);
    }

    /**
     * 内部的に綺麗に構造化して管理している内容を１つの整形された連結テキストにする。
     * @param resultStructArray 
     */
    protected ConvertStructArrayToFormattedString(resultStructArray: IRensoTaigigoElement[]): string {
        let formattedString: string = "";
        for (var element of resultStructArray) {
            formattedString += `${element.word}\r\n`;
        }

        return formattedString;
    }
}

/**
 * 連想対義語用に導出されたクラス
 */
class RensoTaigigoTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {

    protected InitializeQueryParams(): void {
        // 何もしない
    }

    get Method(): QueryMethodType {
        return "POST"; // GETでもPOSTでも両方動作する。
    }

    get Url(): string {
        return "http://thesaurus.weblio.jp/antonym/content/" + this.HttpUtility.HtmlEncode(this.SrcText);
    }

    /**
     * 結果のページはHTML。泥臭く抽出する必要があるが、
     * 適当こくのではなく、イタラブルなオブジェクトとして構築する
     * @param result_page
     */
    FilterResultText(result_page: string): string {
        let rensoTaigigoElementList: IterableRensoTaigigoElementList = new IterableRensoTaigigoElementList(result_page);
        /*
        for(let r: IRensoTaigigoElement of rensoTaigigoElementList) {
            console.log(r);
        }
        */
        return rensoTaigigoElementList.formattedString();
    }
}


// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "ja", dst: "ja" }; // 特に意味はない
// let result_words: string = ContextTranslator.Translate(RensoTaigigoTranslatorQueryStrategy, langParams);