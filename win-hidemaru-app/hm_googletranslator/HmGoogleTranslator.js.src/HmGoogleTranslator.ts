///<reference path="hmv8.d.ts"/>

type TranslatorLanguageType = undefined | "en" | "ja";

type QueryMethodType = "POST" | "GET";

/**
 * 翻訳対象のFromとTo。
 */
interface ITranslatorLanguageParams {
    src: TranslatorLanguageType;
    dst: TranslatorLanguageType;
}

/**
 * 翻訳戦略インターフェイスのコンストラクタ専用インターフェイス
 * ファクトリ系を作る際にはTypeScriptでは分ける必要がある。
 */
interface ITranslatorAlgorithmStrategyConstructor {
    new (targetLanguages: ITranslatorLanguageParams);
}
/**
 * 翻訳戦略のインターフェイス
 */
interface ITranslatorAlgorithmStrategy {
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
abstract class ITranslatorAlgorithm implements ITranslatorAlgorithmStrategy {

    // クエリーする際のkeyとvalueの組み合わせ
    protected queryParams: INameValueCollection;
    get QueryParams(): INameValueCollection {
        return this.queryParams;
    }

    // 翻訳対象の言語のsrcとdst
    protected targetLanguages: ITranslatorLanguageParams;

    // .NETのHttpUtilityは何かとつかうだろうからショートカット用意
    protected HttpUtility: IHttpUtility;

    constructor(targetLanguages: ITranslatorLanguageParams) {
        let webclr: any = host.lib("System.Web");
        this.HttpUtility = webclr.System.Web.HttpUtility;

        this.queryParams = new clr.System.Collections.Specialized.NameValueCollection();
        this.targetLanguages = targetLanguages;

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

    get Method(): QueryMethodType {
        throw "NotImplementsException";
    }

    /**
     * そのサイトへと要求するクエリーパラメータ
     */
    protected InitializeQueryParams(): void {
        throw "NotImplementsException";
    }

    FilterResultText(html: string): string {
        throw "NotImplementsException";
    }
}

/**
 * Google用に導出されたクラス
 */
class GoogleTranslatorAlgorithmStrategy extends ITranslatorAlgorithm {

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
     * 結果のHTMLページから対象の部分を抽出するどろ臭い部分
     * @param html
     */
    FilterResultText(html: string): string {

        // id=result_boxあたりのタグ内のInnerTextが翻訳語の文字列。
        // 本格的にやるなら「HtmlAgilityPack」で、対象のidタグのInnerTextを求めるのが良いが大仰である。
        // 正規表現で対処してしまって良いだろう。
        // 複数行考慮
        let resultAroundTextRegexp: RegExp = /<span id=result_box [\s\S]+?<\/span><\/span>/;

        // 結果のHTMLページ全体から、翻訳結果周辺をまずは抽出。
        let resultAroundResultArray: RegExpExecArray | null = resultAroundTextRegexp.exec(html);

        // 存在する
        if (resultAroundResultArray) {
            // hm.Edit.TotalText = resultAroundResultArray[0];
            // 得た「周辺テキスト」にたいして…
            let regexpResultText: string = resultAroundResultArray[0];
            // hm.Edit.TotalText = regexpResultText;
            // さらにタグを全部除去。複数行考慮
            regexpResultText = regexpResultText.replace(/<("[^"]*"|'[^']*'|[^'">])*>/g, "");

            // 結果文字列がHTMLエンコードされてるのでHTMLデコードする。
            // (ただし元々がHTMLエンコードした状態の文字列と区別が付かないという不正確さがある)
            regexpResultText = this.HttpUtility.HtmlDecode(regexpResultText);

            return regexpResultText;

        } else {
            return "";
        }
    }
}


/**
 * メイン処理的な翻訳全体の大流アルゴリズム
 * 「Web翻訳とは何か」といった共通観念的処理が記載されている。
 */
class ContextTranslator {
    private strategy: ITranslatorAlgorithmStrategy;
    private webclient: IWebClient;

    /**
     * @param strategy : 特定の翻訳戦略アルゴリズムを受け取る
     */
    constructor(strategy: ITranslatorAlgorithmStrategy) {
        this.strategy = strategy;
        this.webclient = new clr.System.Net.WebClient();
    }

    /**
     * 大流としての翻訳。
     * WebClientを使って、strategyの各種パラメータを使ってクエリーをWebへと投げる。
     * 返ってきた値を、stragetyのフィルター方法に従って最終系にする。
     */
    Translate(): void {
        // 翻訳元のデータがないってのはさすがにダメすぎでしょう。
        if (!this.strategy.SrcText) {
            PrintOutputPane("翻訳対象が存在していません。\r\n(対象テキストを選択してない等)\r\n");
            return;
        }

        // クエリを発行した結果の受信データ全体。型はcli::array<Byte>^ だが、面倒くさいのでanyで
        let resData: any = this.GetRequestQueryData();
        if (!resData) {
            PrintOutputPane("翻訳結果をWebへとリクエストしましたが、失敗しました。\r\n");
            return;
        }

        // 結果のデータの塊はUTF8のテキストとみなして変換
        let resText: string = clr.System.Text.Encoding.UTF8.GetString(resData);
        // hm.Edit.TotalText = resText;
        // 結果のテキストを、戦略アルゴリズムが持つ「結果フィルターメソッド」にかける
        let dstText: string = this.strategy.FilterResultText(resText);

        // 翻訳後のテキストを該当の戦略アルゴリズムのDstTextとして設定。
        this.strategy.DstText = dstText;
    }

    private GetRequestQueryData(): any {
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

// 特定の翻訳戦略アルゴリズムや、言語指定パラメータを引数にする。
function Main(ctor: ITranslatorAlgorithmStrategyConstructor, langParams: ITranslatorLanguageParams) {

    // 特定の翻訳戦略アルゴリズムインスタンスを構築
    // ちょっとファクトリっぽい
    let strategy: ITranslatorAlgorithmStrategy = new ctor(langParams);

    // 個別戦略にとらわれない翻訳トランスレーター大流処理インスタンス
    let translator: ContextTranslator = new ContextTranslator(strategy);

    // 翻訳
    translator.Translate();
}

// 何語から何語なのか
let langParams: ITranslatorLanguageParams = { src: "en", dst: "ja" };
Main(GoogleTranslatorAlgorithmStrategy, langParams);