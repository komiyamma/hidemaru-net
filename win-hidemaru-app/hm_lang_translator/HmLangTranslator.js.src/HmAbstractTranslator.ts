///<reference path="HmV8.d.ts"/>

/**
 * HmAbstractTranslator v1.01
 * Copyright (C) @ 2017 VSCode.life
 */

type TranslatorLanguageType = undefined | "auto" | "en" | "ja"; // 実際にはこれらに縛られない。

type QueryMethodType = "POST" | "GET";

/**
 * 翻訳対象の言語のSource(From)とDestination(To)
 */
interface ITranslatorLanguageParams {
    src: TranslatorLanguageType;
    dst: TranslatorLanguageType;
}

/**
 * 翻訳戦略インターフェイスのコンストラクタ専用インターフェイス
 * ファクトリ系を作る際にはTypeScriptでは分ける必要がある。
 */
interface ITranslatorQueryStrategyConstructor {
    new (targetLanguages: ITranslatorLanguageParams);
}
/**
 * 翻訳戦略のインターフェイス
 */
interface ITranslatorQueryStrategy {
    FilterResultText: (html: string) => string;
    QueryParams: INameValueCollection;
    Url: string;
    Method: QueryMethodType;
    SrcText: string;
    DstText: string;
}

/**
 * ストラテジーっぽい翻訳アルゴリズムの抽象クラス。
 */
abstract class AbstractTranslatorQueryStrategy implements ITranslatorQueryStrategy {

    // クエリーする際のkeyとvalueの組み合わせ
    protected m_queryParams: INameValueCollection;
    get QueryParams(): INameValueCollection {
        return this.m_queryParams;
    }

    // 翻訳対象の言語のsrcとdst
    protected m_targetLanguages: ITranslatorLanguageParams;
    get TargetLanguages(): ITranslatorLanguageParams {
        return this.m_targetLanguages;
    }

    // .NETのHttpUtilityは何かとつかうだろうからショートカット用意
    protected HttpUtility: IHttpUtility;

    constructor(targetLanguages: ITranslatorLanguageParams) {
        let webclr: any = host.lib("System.Web");
        this.HttpUtility = webclr.System.Web.HttpUtility;

        this.m_queryParams = new clr.System.Collections.Specialized.NameValueCollection();
        this.m_targetLanguages = targetLanguages;

        this.InitializeQueryParams();
    }

    get Url(): string {
        throw "NotImplementsException";
    }

    /**
     * デフォルトは通常選択や行選択に対するテキスト。
     * 矩形選択も対象にしたければ、少し記述を足す必要があるが、あまり機会は無いだろう。
     */
    get SrcText(): string {
        return hm.Edit.SelectedText;
    }

    /**
     * 結果を反映する時のフォーマット。
     * どのような文字列フォーマットで結果を足しこんでいくか。
     */
    set DstText(value: string) {
        hm.Edit.SelectedText = `${this.SrcText}\n${value}\n`;
    }

    /**
     * メソッド。通常GETかPOSTを想定。将来的にAUTHを足してもいいかもしれない。
     */
    get Method(): QueryMethodType {
        throw "NotImplementsException";
    }

    /**
     * そのサイトへと要求するクエリーパラメータ
     */
    protected InitializeQueryParams(): void {
        throw "NotImplementsException";
    }

    /**
     * 結果のページから対象の部分を抽出するどろ臭い部分
     * HTMLかもしれないし、XMLかもしれないし、JSONかもしれない。
     * 検索エンジンによって異なるだろう。
     */
    FilterResultText(result_page: string): string {
        throw "NotImplementsException";
    }
}

/**
 * メイン処理的な翻訳全体の大流アルゴリズム
 * 「Web翻訳とは何か」といった共通観念的処理が記載されている。
 */
class ContextTranslator {
    protected strategy: ITranslatorQueryStrategy;
    protected webclient: IWebClient;

    /**
     * @param strategy : 特定の翻訳戦略アルゴリズムを受け取る
     */
    protected constructor(strategy: ITranslatorQueryStrategy) {
        this.strategy = strategy;
        this.webclient = new clr.System.Net.WebClient();
    }

    /**
     * 大流としての翻訳。
     * WebClientを使って、strategyの各種パラメータを使ってクエリーをWebへと投げる。
     * 返ってきた値を、stragetyのフィルター方法に従って最終系にする。
     */
    protected GetTranslateResult(): string {
        // 翻訳元のデータがないってのはさすがにダメすぎでしょう。
        if (!this.strategy.SrcText) {
            PrintOutputPane("翻訳対象が存在していません。\r\n(対象テキストを選択してない等)\r\n");
            return "";
        }

        // クエリを発行した結果の受信データ全体。型はcli::array<Byte>^ だが、面倒くさいのでanyで
        let resData: any = this.GetRequestQueryData();
        if (!resData) {
            PrintOutputPane("翻訳結果をWebへとリクエストしましたが、失敗しました。\r\n");
            return "";
        }

        // 結果のデータの塊はUTF8のテキストとみなして変換
        let resText: string = clr.System.Text.Encoding.UTF8.GetString(resData);
        // hm.Edit.TotalText = resText;
        // 結果のテキストを、戦略アルゴリズムが持つ「結果フィルターメソッド」にかける
        let dstText: string = this.strategy.FilterResultText(resText);

        return dstText;
    }

    protected GetRequestQueryData(): any {
        // 戦略アルゴリズムが持つクエリーパラメータ
        let queryParams =  this.strategy.QueryParams;

        // 戦略のクエリー手段(GET/POST)
        let method: QueryMethodType = this.strategy.Method;

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
    static Translate(ctor: ITranslatorQueryStrategyConstructor, langParams: ITranslatorLanguageParams): string {

        // 特定の翻訳戦略アルゴリズムインスタンスを構築
        // ちょっとファクトリっぽい
        let strategy: ITranslatorQueryStrategy = new ctor(langParams);

        // 個別戦略にとらわれない翻訳トランスレーター大流処理インスタンス
        let translator: ContextTranslator = new ContextTranslator(strategy);

        // 翻訳結果を取得し、反映
        let result: string = translator.GetTranslateResult();

        strategy.DstText = result;

        return result; 
    }
    
}

/**
 * 秀丸用のアウトプットパネル。簡易
 */
function PrintOutputPane(message: string): void {
    // tslint:disable-next-line:no-string-literal
    hm.Macro.Var["$OutputPaneMessageText"] = message;
    hm.Macro.Eval(f => {
        /*
            #HMOP = loaddll(hidemarudir + @"\HmOutputPane.dll");
            #r = dllfunc(#HMOP, "Output", hidemaruhandle(0), $OutputPaneMessageText + "\r\n");
            freedll( #HMOP );
        */
    });
}
