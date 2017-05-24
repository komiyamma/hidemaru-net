///<reference path="HmV8.d.ts"/>
///<reference path="HmAbstractTranslator.ts"/>
/**
 * HmRensoRuigoTranslator v1.02
 * Copyright (C) 2017 VSCode.life
 */

interface IRensoRuigoElement {
    word: string; // 連想類語
    href: string; // その単語でさらに連想類語検索する際のURL
}

class IterableRensoRuigoElementList {
    m_resultStructArray: IRensoRuigoElement[];

    constructor(result_page: string) {
        this.m_resultStructArray = this.ConvertStringToStructArray(result_page);
    }

    /**
     * づらずらと未フォーマットな文字列になっているので構造を把握しやすい配列へと格納する。
     * @param result_page 
     */
    protected ConvertStringToStructArray(result_page: string): IRensoRuigoElement[] {
        // 途中の改行は面倒くさいので除去
        let simgleResultPage: string  = result_page.replace(/[\r\n]/, "");

        // word_t_fieldのタグ内が答えのリスト一覧
        let resultAroundTextRegexp: RegExp = /<div class="word_t_field">([\s\S]+?)<\/div>/;
        let resultAroundExecArray: RegExpExecArray | null = resultAroundTextRegexp.exec(simgleResultPage);

        // １つも無い
        if (!resultAroundExecArray) {
            return [];
        }

        let joinedRensoWordText: string = resultAroundExecArray[0];

        let resultStructArray:IRensoRuigoElement[] = [];
        // 結果パターンの繰り返し
        let resultTextRegexp: RegExp = /　・　<a href="([\s\S]+?)">([\s\S]+?)<\/a>/g;
        let resultExecArray: RegExpExecArray | null;
        // １つずつ抽出して、IRensoRuigoElement型にして IRensoRuigoElement配列へと足し込み
        while(resultExecArray = resultTextRegexp.exec(joinedRensoWordText)) {
            if (resultExecArray) {
                // この形にして格納しておく。
                let element: IRensoRuigoElement = {word:resultExecArray[2], href:resultExecArray[1]};
                resultStructArray.push( element );
            }
        }

        return resultStructArray;
    }

    /**
     * イテレータ。
     */
    * [Symbol.iterator](): IterableIterator<IRensoRuigoElement> {
        for (let i: number = 0; i<this.m_resultStructArray.length; i++) {
            yield this.m_resultStructArray[i];
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
    protected ConvertStructArrayToFormattedString(resultStructArray: IRensoRuigoElement[]): string {
        // 結果はアルファベットも含め「全部全角」なので、リスト文字列の中で一番長い文字数を求める。
        let lenArray: number[] = resultStructArray.map( o => o.word.length );
        let maxLen: number = Math.max(...lenArray);

        // resultArrayを１つの文字列へ
        let formattedString: string = "";
        for (var element of resultStructArray) {

            // 以下、「左に単語」「右にリンク」への整形
            // 左の単語部分の空白調整が不細工で「うぐぐ…」。

            // 残り幅 = 最大幅 - 現在の単語の幅
            let restSpaceCnt:number = (maxLen - element.word.length) * 2;
            // 半角の(と)の数を調査して、その分は半角なので、足しておく。それ以外の半角文字は出てこないと思われる。
            let halfSizeCharCnt: number = ( element.word.match(/[\(\)]/g) || [] ).length;
            restSpaceCnt = restSpaceCnt + halfSizeCharCnt;
            // 残り幅分の空白を現在の単語の最後に追加しておく。
            let formatDst: string = element.word + " ".repeat(restSpaceCnt);

            formattedString += `${formatDst} : ${element.href}\r\n`;
        }

        return formattedString;
    }
}

/**
 * 連想類語用に導出されたクラス
 */
class RensoRuigoTranslatorQueryStrategy extends AbstractTranslatorQueryStrategy {

    protected InitializeQueryParams(): void {
        // 何もしない
    }

    get Method(): QueryMethodType {
        return "POST"; // GETでもPOSTでも両方動作する。
    }

    get Url(): string {
        return "http://renso-ruigo.com/word/" + this.HttpUtility.HtmlEncode(this.SrcText);
    }

    /**
     * 結果のページはHTML。泥臭く抽出する必要があるが、
     * 適当こくのではなく、イタラブルなオブジェクトとして構築する
     * @param result_page
     */
    FilterResultText(result_page: string): string {
        let rensoRuigoElementList: IterableRensoRuigoElementList = new IterableRensoRuigoElementList(result_page);
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