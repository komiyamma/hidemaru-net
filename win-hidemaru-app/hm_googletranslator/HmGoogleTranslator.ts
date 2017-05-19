///<reference path="hmv8.d.ts"/>

/**
 * 秀丸用のアウトプットパネル。簡易
 */
class OutputPane {
    // 秀丸のアウトプット枠への文字列出力
    static print(message: string): void {
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
}

type TranslatorLanguageType = undefined | "en" | "ja";

type QueryMethodType = "POST" | "GET";

/**
 * 翻訳対象のFromとTo。interfaceにすれば良いのだけれども、
 * interfaceだとES6にトランスパイルすると定義が消えるので抽象クラスで。
 */
abstract class ITranslatorLanguageParams {
    src: TranslatorLanguageType = undefined;
    dst: TranslatorLanguageType = undefined;
}

/**
 * 翻訳戦略のインターフェイス
 */
interface ITranslatorAlgorithmStrategy {
    InitializeQueryParams: () => void;
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
        hm.Edit.SelectedText = `${this.SrcText}\n（${value}）\n`;
    }

    get Method(): QueryMethodType {
        throw "NotImplementsException";
    }

    /**
     * そのサイトへと要求するクエリーパラメータ
     */
    InitializeQueryParams(): void {
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

    InitializeQueryParams(): void {

        // 翻訳対象の元テキスト。自動でUTF-8にURLエンコードされる。
        this.queryParams.Add("text", this.SrcText);

        // 何語から何語へ翻訳するのか
        this.queryParams.Add("langpair", `${this.targetLanguages.src}|${this.targetLanguages.dst}`);

        // 結果ページで使用される言語
        this.queryParams.Add("hl", this.targetLanguages.dst);

        // 入力文字列の文字コード
        this.queryParams.Add("ie", "UTF8");
    }

    get Method(): QueryMethodType {
        return "POST";
    }

    get Url(): string {
        return "http://translate.google.com/translate_t";
    }

    /**
     * 結果のHTMLページから対象の部分を抽出するどろ臭い部分
     * @param html
     */
    FilterResultText(html: string): string {
        // id=result_boxあたりのタグ内のInnerTextに含まれる
        let resultAroundTextRegexp: RegExp = /<span id=result_box .+?<\/span>/;

        // 結果のHTMLページ全体から、翻訳結果周辺をまずは抽出
        let resultAroundResultArray: RegExpExecArray | null = resultAroundTextRegexp.exec(html);

        // 存在する
        if (resultAroundResultArray) {
            // 得た「周辺テキスト」にたいして…
            let regexpResultText: string = resultAroundResultArray[0];

            // さらにタグを全部除去
            let removeTagRegexp: RegExp = new RegExp("</?[^>]+>", "g");
            regexpResultText = regexpResultText.replace(removeTagRegexp, "");

            return regexpResultText;
        }
        return "";
    }
}


/**
 * メイン処理的な翻訳全体の大流アルゴリズム
 * strategyに特定の翻訳アルゴリズムを放り込む
 */
class ContextTranslator {
    strategy: ITranslatorAlgorithmStrategy;
    webclient: IWebClient;

    /**
     * @param strategy : 特定の翻訳戦略アルゴリズムを受け取る
     */
    constructor(strategy: ITranslatorAlgorithmStrategy) {
        this.webclient = new clr.System.Net.WebClient();
        this.strategy = strategy;
    }

    /**
     * 大流れとしての翻訳。
     * WebClientを使って、strategyの各種パラメータを使ってクエリーをWebへと投げる。
     * 返ってきた値を、stragetyのフィルター方法に従って最終系にする。
     */
    Translate(): void {
        // 戦略アルゴリズムが持つクエリーパラメータ
        let queryParams: INameValueCollection = this.strategy.QueryParams;

        // クエリを発行した結果の受信データ全体。型はcli::array<Byte>^ だが、面倒くさいのでanyで
        let resData: any = this.GetRequestQueryData(queryParams);
        if (!resData) {
            return;
        }

        // 結果のデータの塊はUTF8のテキストとみなして変換
        let resText: string = clr.System.Text.Encoding.UTF8.GetString(resData);

        // 結果のテキストを、戦略アルゴリズムが持つ「結果フィルターメソッド」にかける
        let dstText: string = this.strategy.FilterResultText(resText);

        // 翻訳後のテキストを該当の戦略アルゴリズムのDstTextとして設定。
        this.strategy.DstText = dstText;
    }

    private GetRequestQueryData(queryParams: INameValueCollection): any {
        // 戦略のクエリー手段(GET/POST)
        let method: QueryMethodType = this.strategy.Method;

        // メソッドがPOSTとGETで少々要求メソッドや設定方法が違うので、ちょっとブサだがswitch
        switch (method) {
            case "POST": {
                this.webclient.Headers.Add("Content-Type", "application/x-www-form-urlencoded");
                return this.webclient.UploadValues(this.strategy.Url, queryParams);
            }
            case "GET": {
                this.webclient.Headers.Add("User-Agent", "Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko");
                this.webclient.QueryString = queryParams;
                return this.webclient.DownloadData(this.strategy.Url);
            }
        }
    }
}

// 何語から何語なのか
let langParams: ITranslatorLanguageParams = { src: "en", dst: "ja" };

// 言語指定パラメータを引数にしつつ、特定の翻訳戦略アルゴリズムインスタンスを構築
// ここはファクトリにした方がいいけど…まぁ翻訳サイトでまともなクオリティのものは少ないので
// 直書きでもいいでしょう。
let strategy: GoogleTranslatorAlgorithmStrategy = new GoogleTranslatorAlgorithmStrategy(langParams);

// 個別戦略にとらわれない翻訳トランスレーター大流処理インスタンス
let translator: ContextTranslator = new ContextTranslator(strategy);

// 翻訳
translator.Translate();
