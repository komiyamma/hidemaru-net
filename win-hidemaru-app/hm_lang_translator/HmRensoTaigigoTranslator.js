///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmRensoTaigigoTranslator v1.03
 * Copyright (C) 2017-2019 Akitsugu Komiyama
 * under the MIT License
 */
class IterableRensoTaigigoElementList {
    constructor(result_page) {
        this.m_resultStructArray = this.ConvertStringToStructArray(result_page);
    }
    /**
     * づらずらと未フォーマットな文字列になっているので構造を把握しやすい配列へと格納する。
     * @param result_page
     */
    ConvertStringToStructArray(result_page) {
        // 途中の改行は面倒くさいので除去
        let simgleResultPage = result_page.replace(/[\r\n]/, "");
        // word_t_fieldのタグ内が答えのリスト一覧
        let resultAroundTextRegexp = /<div class=Wtght>([\s\S]+?)<\/div><\/div>/;
        let resultAroundExecArray = resultAroundTextRegexp.exec(simgleResultPage);
        // １つも無い
        if (!resultAroundExecArray) {
            return [];
        }
        let joinedRensoWordText = resultAroundExecArray[0];
        console.log(joinedRensoWordText);
        let resultStructArray = [];
        // 結果パターンの繰り返し
        let resultTextRegexp = /<td class=wtghtTW><span class=wtghtAntnm>(.+?)<\/span>[\s\S]+?<br class=clr>/g;
        let resultExecArray;
        // １つずつ抽出して、IRensoTaigigoElement型にして IRensoTaigigoElement配列へと足し込み
        while (resultExecArray = resultTextRegexp.exec(joinedRensoWordText)) {
            let word = resultExecArray[1];
            // さらにタグを全部除去。複数行考慮
            word = word.replace(/<("[^"]*"|'[^']*'|[^'">])*>/g, "");
            let element = { word: word };
            resultStructArray.push(element);
        }
        return resultStructArray;
    }
    /**
     * イテレータ。
     */
    *[Symbol.iterator]() {
        for (let e of this.m_resultStructArray) {
            yield e;
        }
    }
    /**
     * 内部的に綺麗に構造化して管理している内容を１つの整形された連結テキストにする。
     */
    formattedString() {
        return this.ConvertStructArrayToFormattedString(this.m_resultStructArray);
    }
    /**
     * 内部的に綺麗に構造化して管理している内容を１つの整形された連結テキストにする。
     * @param resultStructArray
     */
    ConvertStructArrayToFormattedString(resultStructArray) {
        let formattedString = "";
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
    InitializeQueryParams() {
        // 何もしない
    }
    get Method() {
        return "POST"; // GETでもPOSTでも両方動作する。
    }
    get Url() {
        return "http://thesaurus.weblio.jp/antonym/content/" + this.HttpUtility.HtmlEncode(this.SrcText);
    }
    /**
     * 結果のページはHTML。泥臭く抽出する必要があるが、
     * 適当こくのではなく、イタラブルなオブジェクトとして構築する
     * @param result_page
     */
    FilterResultText(result_page) {
        let rensoTaigigoElementList = new IterableRensoTaigigoElementList(result_page);
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
