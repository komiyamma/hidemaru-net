///<reference path="hmv8.d.ts"/>
/**
 * ストラテジーっぽい翻訳アルゴリズムの抽象クラス。
 */
class ITranslatorAlgorithm {
    get QueryParams() {
        return this.queryParams;
    }
    constructor(targetLanguages) {
        let webclr = host.lib("System.Web");
        this.HttpUtility = webclr.System.Web.HttpUtility;
        this.queryParams = new clr.System.Collections.Specialized.NameValueCollection();
        this.targetLanguages = targetLanguages;
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
    get Method() {
        throw "NotImplementsException";
    }
    /**
     * そのサイトへと要求するクエリーパラメータ
     */
    InitializeQueryParams() {
        throw "NotImplementsException";
    }
    FilterResultText(html) {
        throw "NotImplementsException";
    }
}
/**
 * Google用に導出されたクラス
 */
class GoogleTranslatorAlgorithmStrategy extends ITranslatorAlgorithm {
    InitializeQueryParams() {
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
    get Method() {
        return "GET";
    }
    get Url() {
        return "http://translate.google.com/translate_t";
    }
    /**
     * 結果のHTMLページから対象の部分を抽出するどろ臭い部分
     * @param html
     */
    FilterResultText(html) {
        // id=result_boxあたりのタグ内のInnerTextが翻訳語の文字列。
        // 本格的にやるなら「HtmlAgilityPack」で、対象のidタグのInnerTextを求めるのが良いが大仰である。
        // 正規表現で対処してしまって良いだろう。
        // 複数行考慮
        let resultAroundTextRegexp = /<span id=result_box [\s\S]+?<\/span><\/span>/;
        // 結果のHTMLページ全体から、翻訳結果周辺をまずは抽出。
        let resultAroundResultArray = resultAroundTextRegexp.exec(html);
        // 存在する
        if (resultAroundResultArray) {
            // hm.Edit.TotalText = resultAroundResultArray[0];
            // 得た「周辺テキスト」にたいして…
            let regexpResultText = resultAroundResultArray[0];
            // hm.Edit.TotalText = regexpResultText;
            // さらにタグを全部除去。複数行考慮
            regexpResultText = regexpResultText.replace(/<("[^"]*"|'[^']*'|[^'">])*>/g, "");
            // 結果文字列がHTMLエンコードされてるのでHTMLデコードする。
            // (ただし元々がHTMLエンコードした状態の文字列と区別が付かないという不正確さがある)
            regexpResultText = this.HttpUtility.HtmlDecode(regexpResultText);
            return regexpResultText;
        }
        else {
            return "";
        }
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
        this.webclient = new clr.System.Net.WebClient();
        this.strategy = strategy;
    }
    /**
     * 大流としての翻訳。
     * WebClientを使って、strategyの各種パラメータを使ってクエリーをWebへと投げる。
     * 返ってきた値を、stragetyのフィルター方法に従って最終系にする。
     */
    Translate() {
        // クエリを発行した結果の受信データ全体。型はcli::array<Byte>^ だが、面倒くさいのでanyで
        let resData = this.GetRequestQueryData();
        if (!resData) {
            return;
        }
        // 結果のデータの塊はUTF8のテキストとみなして変換
        let resText = clr.System.Text.Encoding.UTF8.GetString(resData);
        // hm.Edit.TotalText = resText;
        // 結果のテキストを、戦略アルゴリズムが持つ「結果フィルターメソッド」にかける
        let dstText = this.strategy.FilterResultText(resText);
        // 翻訳後のテキストを該当の戦略アルゴリズムのDstTextとして設定。
        this.strategy.DstText = dstText;
    }
    GetRequestQueryData() {
        // 翻訳元のデータがないってのはさすがにダメすぎでしょう。
        if (!this.strategy.SrcText) {
            PrintOutputPane("翻訳対象が存在していません。\r\n(対象テキストを選択してない等)\r\n");
            return;
        }
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
}
/**
 * 秀丸用のアウトプットパネル。簡易
 */
function PrintOutputPane(message) {
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
function Main(ctor, langParams) {
    // 特定の翻訳戦略アルゴリズムインスタンスを構築
    // ちょっとファクトリっぽい
    let strategy = new ctor(langParams);
    // 個別戦略にとらわれない翻訳トランスレーター大流処理インスタンス
    let translator = new ContextTranslator(strategy);
    // 翻訳
    translator.Translate();
}
// 何語から何語なのか
let langParams = { src: "en", dst: "ja" };
Main(GoogleTranslatorAlgorithmStrategy, langParams);
