declare namespace hm {
 
    /**
     * 秀丸のバージョン。
     */
    const version: number;
                 
    /**
     * デバッグモニタへの出力。console.logと同じ。
     */
    function debuginfo(message?: any, ...optionalParams: any[]): void;
 
    namespace File {
        interface IEncoding {
            readonly HmEncode: number;
            readonly MsCodePage: number;
            readonly JsEncodingName: string;
        }
 
        interface IHidemaruStreamReader {
            readonly FilePath: string;
            readonly Encoding: IEncoding;
            Read(): string;
            Close(): void;
        }
 
        /**
         * 対象の外部ファイルを秀丸で「はじめて」オープンした際、あなたの設定の秀丸だとどのような文字コードだと判定するか
         */
        function GetEncoding(filepath: string): IEncoding;
 
        /**
         * 対象の外部ファイルを秀丸で「はじめて」オープンした際、あなたの設定の秀丸で開いた時と同じ状態でオープン
         */
        function Open(filepath: string, hm_encode?: number): IHidemaruStreamReader;
    }
 
    namespace Macro {
 
        /**
         * 複数の秀丸マクロをヒアドキュメント的に記述するための特殊な関数表記の型。
         */
        type TFnExpression = (f: any) => void;
 
        /**
         * 秀丸マクロを実行。
         */
        function Eval(expression: string | TFnExpression): {
            readonly Result: number;
            readonly Message: string;
            readonly Error: {
                readonly Message: string;
            }
        };
 
        /**
         * 秀丸マクロの変数の読み書きのための型。関数のような表記と連想配列のような表記。
         */
        interface TVar {
            (key: string): number | string;
            (key: string, value: any): void;
            [key: string]: any;
        }
 
        /**
         * 秀丸マクロの変数の読み書き。
         */
        let Var: TVar;
    }
 
    namespace Edit {
 
        /**
         * 編集中のテキスト内容の読み書き
         */
        let TotalText: string;
 
        /**
         * 通常選択や行選択のテキスト内容の読み書き
         */
        let SelectedText: string;
 
        /**
         * カーソル行のテキスト内容の読み書き
         */
        let LineText: string;
 
        /**
         * カーソルの位置
         */
        const CursorPos: {
            readonly column: number;
            readonly lineno: number;
        };
 
        /**
         * マウスの位置に対応するカーソルの位置
         */
        const MousePos: {
            readonly column: number;
            readonly lineno: number;
            readonly x: number;
            readonly y: number;
        };
    }

    namespace OutputPane {
 
        /**
         * アウトプット枠への出力
         */
         function Output(message: string): number;
 
        /**
         * 一時退避
         */
         function Push(): number;

        /**
         * 復元
         */
         function Pop(): number;

         /**
         * クリア
         */
         function Clear(): number;

         /**
         * アウトプット枠へのコマンド実行
         */
         function SendMessage(command_id: number): number;

         /**
         * ベースディレクトリの設定
         */
         function SetBaseDir(dirpath: string): number;
    }

}