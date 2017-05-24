///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmRensoRuigoTranslator v1.02
 * Copyright (C) 2017 VSCode.life
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
        let resultTextRegexp = /　・　<a href="([\s\S]+?)">([\s\S]+?)<\/a>/g;
        let resultExecArray;
        // １つずつ抽出して、IRensoRuigoElement型にして IRensoRuigoElement配列へと足し込み
        while (resultExecArray = resultTextRegexp.exec(joinedRensoWordText)) {
            if (resultExecArray) {
                // この形にして格納しておく。
                let element = { word: resultExecArray[2], href: resultExecArray[1] };
                resultStructArray.push(element);
            }
        }
        return resultStructArray;
    }
    /**
     * イテレータ。
     */
    *[Symbol.iterator]() {
        for (let i = 0; i < this.m_resultStructArray.length; i++) {
            yield this.m_resultStructArray[i];
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
        // 結果はアルファベットも含め「全部全角」なので、リスト文字列の中で一番長い文字数を求める。
        let lenArray = resultStructArray.map(o => o.word.length);
        let maxLen = Math.max(...lenArray);
        // resultArrayを１つの文字列へ
        let formattedString = "";
        for (var element of resultStructArray) {
            // 以下、「左に単語」「右にリンク」への整形
            // 左の単語部分の空白調整が不細工で「うぐぐ…」。
            // 残り幅 = 最大幅 - 現在の単語の幅
            let restSpaceCnt = (maxLen - element.word.length) * 2;
            // 半角の(と)の数を調査して、その分は半角なので、足しておく。それ以外の半角文字は出てこないと思われる。
            let halfSizeCharCnt = (element.word.match(/[\(\)]/g) || []).length;
            restSpaceCnt = restSpaceCnt + halfSizeCharCnt;
            // 残り幅分の空白を現在の単語の最後に追加しておく。
            let formatDst = element.word + " ".repeat(restSpaceCnt);
            formattedString += `${formatDst} : ${element.href}\r\n`;
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
