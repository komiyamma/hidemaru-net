///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmRensoRuigoTranslator v1.02
 * Copyright (C) 2017-2019 Akitsugu Komiyama
 * under the MIT License
 */
class IterableRensoRuigoElementList {
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
        let resultAroundTextRegexp = /<div class="word_t_field">([\s\S]+?)<\/div>/;
        let resultAroundExecArray = resultAroundTextRegexp.exec(simgleResultPage);
        // １つも無い
        if (!resultAroundExecArray) {
            return [];
        }
        let joinedRensoWordText = resultAroundExecArray[0];
        let resultStructArray = [];
        // 結果パターンの繰り返し
        let resultTextRegexp = /(?:　・　)?<a href="([\s\S]+?)">([\s\S]+?)<\/a>/g;
        let resultExecArray;
        // １つずつ抽出して、IRensoRuigoElement型にして IRensoRuigoElement配列へと足し込み
        while (resultExecArray = resultTextRegexp.exec(joinedRensoWordText)) {

            let modified_word = resultExecArray[2]
            modified_word = modified_word.replace(/<rp>（<\/rp><rt>.+?<\/rt><rp>）<\/rp>/g, "");
            modified_word = modified_word.replace(/<\/?ruby>/g, "");
            // この形にして格納しておく。
            let element = { word: modified_word, href: resultExecArray[1] };
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
            // formattedString += `${element.word} : ${element.href}\r\n`;
            formattedString += `${element.word}\r\n`;
        }
        return formattedString;
    }
}
/**
 * 連想類語用に導出されたクラス
 */
class RensoRuigoTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {
    InitializeQueryParams() {
        // 何もしない
    }
    get Method() {
        return "POST"; // GETでもPOSTでも両方動作する。
    }
    get Url() {
        return "http://renso-ruigo.com/word/" + this.HttpUtility.HtmlEncode(this.SrcText);
    }
    /**
     * 結果のページはHTML。泥臭く抽出する必要があるが、
     * 適当こくのではなく、イタラブルなオブジェクトとして構築する
     * @param result_page
     */
    FilterResultText(result_page) {
        let rensoRuigoElementList = new IterableRensoRuigoElementList(result_page);
        /*
        for(let r: IRensoRuigoElement of rensoRuigoElementList) {
            console.log(r);
        }
        */
        return rensoRuigoElementList.formattedString();
    }
}
// 何語から何語なのか
// let langParams: ITranslatorLanguageParams = { src: "ja", dst: "ja" }; // 特に意味はない
// let result_words: string = ContextTranslator.Translate(RensoRuigoTranslatorQueryStrategy, langParams); 
