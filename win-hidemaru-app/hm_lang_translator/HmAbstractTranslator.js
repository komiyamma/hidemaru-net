///<reference path="HmV8.d.ts"/>
/**
 * HmAbstractTranslator v1.02
 * Copyright (C) 2017 Akitsugu Komiyama
 * under the MIT License
 */
/**
 * ストラテジーっぽい翻訳アルゴリズムの抽象クラス。
 */
class AbstractTranslatorQueryStrategy {
    get QueryParams() {
        return this.m_queryParams;
    }
    get TargetLanguages() {
        return this.m_targetLanguages;
    }
    constructor(targetLanguages) {
        let webclr = host.lib("System.Web");
        this.HttpUtility = webclr.System.Web.HttpUtility;
        this.m_queryParams = new clr.System.Collections.Specialized.NameValueCollection();
        this.m_targetLanguages = targetLanguages;
        this.InitializeQueryParams();
    }
    get Url() {
        throw "NotImplementsException";
    }
    /**
     * デフォルトは通常選択や行選択に対するテキスト。
     * 矩形選択も対象にしたければ、少し記述を足す必要があるが、あまり機会は無いだろう。
     */
    get SrcText() {
        return hm.Edit.SelectedText;
    }
    /**
     * 結果を反映する時のフォーマット。
     * どのような文字列フォーマットで結果を足しこんでいくか。
     */
    set DstText(value) {
        hm.Edit.SelectedText = `${this.SrcText}\n${value}\n`;
    }
    /**
     * メソッド。通常GETかPOSTを想定。将来的にAUTHを足してもいいかもしれない。
     */
    get Method() {
        throw "NotImplementsException";
    }
    /**
     * そのサイトへと要求するクエリーパラメータ
     */
    InitializeQueryParams() {
        throw "NotImplementsException";
    }
    /**
     * 結果のページから対象の部分を抽出するどろ臭い部分
     * HTMLかもしれないし、XMLかもしれないし、JSONかもしれない。
     * 検索エンジンによって異なるだろう。
     */
    FilterResultText(result_page) {
        throw "NotImplementsException";
    }
}
/**
 * メイン処理的な翻訳全体の大流アルゴリズム
 * 「Web翻訳とは何か」といった共通観念的処理が記載されている。
 */
class ContextTranslator {
    /**
     * @param strategy : 特定の翻訳戦略アルゴリズムを受け取る
     */
    constructor(strategy) {
        this.strategy = strategy;
        this.webclient = new clr.System.Net.WebClient();
    }
    /**
     * 大流としての翻訳。
     * WebClientを使って、strategyの各種パラメータを使ってクエリーをWebへと投げる。
     * 返ってきた値を、stragetyのフィルター方法に従って最終系にする。
     */
    GetTranslatedText() {
        // 翻訳元のデータがないってのはさすがにダメすぎでしょう。
        if (!this.strategy.SrcText) {
            PrintOutputPane("翻訳対象が存在していません。\r\n(対象テキストを選択してない等)\r\n");
            return "";
        }
        // クエリを発行した結果の受信データ全体。型はcli::array<Byte>^ だが、面倒くさいのでanyで
        let resData = this.GetRequestQueryData();
        if (!resData) {
            PrintOutputPane("翻訳結果をWebへとリクエストしましたが、失敗しました。\r\n");
            return "";
        }
        // 結果のデータの塊はUTF8のテキストとみなして変換
        let resText = clr.System.Text.Encoding.UTF8.GetString(resData);
        // 結果のテキストを、戦略アルゴリズムが持つ「結果フィルターメソッド」にかける
        let dstText = this.strategy.FilterResultText(resText);
        return dstText;
    }
    GetRequestQueryData() {
        // 戦略アルゴリズムが持つクエリーパラメータ
        let queryParams = this.strategy.QueryParams;
        // 戦略のクエリー手段(GET/POST)
        let method = this.strategy.Method;
        // メソッドがPOSTとGETで少々要求メソッドや設定方法が違うので、ちょっとブサだがswitch
        switch (method) {
            case "POST": {
                this.webclient.Headers.Add("Content-Type", "application/x-www-form-urlencoded");
                return this.webclient.UploadValues(this.strategy.Url, queryParams);
            }
            case "GET": {
                this.webclient.QueryString = queryParams;
                return this.webclient.DownloadData(this.strategy.Url);
            }
        }
    }
    // 特定の翻訳戦略アルゴリズムや、言語指定パラメータを引数にする。
    static Translate(ctor, langParams) {
        // 特定の翻訳戦略アルゴリズムインスタンスを構築
        // ちょっとファクトリっぽい
        let strategy = new ctor(langParams);
        // 個別戦略にとらわれない翻訳トランスレーター大流処理インスタンス
        let translator = new ContextTranslator(strategy);
        // 翻訳結果を取得し、反映
        let result = translator.GetTranslatedText();
        strategy.DstText = result;
        return result;
    }
}
/**
 * 秀丸用のアウトプットパネル。簡易
 */
function PrintOutputPane(message) {
    // tslint:disable-next-line:no-string-literal
    hm.Macro.Var["$OutputPaneMessageText"] = message;
    hm.Macro.Eval(f => { /*
        #HMOP = loaddll(hidemarudir + @"\HmOutputPane.dll");
        #r = dllfunc(#HMOP, "Output", hidemaruhandle(0), $OutputPaneMessageText + "\r\n");
        freedll( #HMOP );
    */ });
}
