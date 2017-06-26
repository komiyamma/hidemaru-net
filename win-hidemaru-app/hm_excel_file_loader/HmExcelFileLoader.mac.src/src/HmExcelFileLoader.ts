///<reference path="HmV8.d.ts"/>
///<reference path="HmExcelFileLoader.d.ts"/>

AddReferenceByPartialName();

/**
 * メイン処理
 */
function Main() {
    // ファイル名は未定（ファイル名が決まっているなら、ここでファイルのフルパスを指定すれば良い）
    let xlsFileFullPath: string = "";

    // そのようなファイル名が存在しないのであれば
    if (!clr.System.IO.File.Exists(xlsFileFullPath)) {
        // ファイル選択ダイアログを出してマニュアルで選択
        xlsFileFullPath = SelectOpenFileDialog();
    }
    // まだファイル名が決まっていなければ、注意メッセージを表示
    if (!xlsFileFullPath) {
        PrintOutputPane("有効なExcelファイルを対象してください。")
        return;
    }
    

    let controller: HmExcelFileLoaderController = new HmExcelFileLoaderController(xlsFileFullPath);

    // 秀丸のCSVモードやTSVモードはセル中の改行に対応していないため、セルの値を加工する。
    // どのようにカスタムしたいかはユーザーに選択の自由があったほうが良いので、このように注入可能としておく。
    // 改行コード→[[改行]]に、" "で囲む。
    controller.OverrideCellNormalizeAlgorithm( (cell: string) => cell.replace(/\n/g, "[[改行]]") );

    // 変換
    let tsvFileNameList: string[] = controller.Convert();

    // 変換したファイル名を、秀丸マクロスコープへと伝達しておく。
    for (let i = 0; i < tsvFileNameList.length; i++) {
        TransmitToHmMacroScope(tsvFileNameList[i], i);
    }

    // 残りはファイルの一番下部。秀丸マクロ側で処理する。
}

/**
 * アセンブリのロード
 */
function AddReferenceByPartialName(): void {
    // このマクロファイルの場所をアセンブリの置き場としても登録する
    let currentmacrodirectory: string = hm.Macro.Var["currentmacrodirectory"];
    AssemblyPath.Add(currentmacrodirectory);
    // アセンブリの読み込み
    host.lib(clr, "HmExcelFileLoader");
}

/**
 * ファイル選択ダイアログを出して、xlsを１つ選択する。
 */
function SelectOpenFileDialog(): string | null {
    let lib = host.lib("System.Windows.Forms");
    let OpenFileDialog: System.Windows.Forms.OpenFileDialog = lib.System.Windows.Forms.OpenFileDialog;

    //OpenFileDialogクラスのインスタンスを作成
    let ofd: System.Windows.Forms.OpenFileDialog = new OpenFileDialog();

    //[ファイルの種類]に表示される選択肢を指定する
    //指定しないとすべてのファイルが表示される
    ofd.Filter = "Excelファイル(*.xls;*.xlsx)|*.xls;*.xlsx|すべてのファイル(*.*)|*.*";
    //[ファイルの種類]ではじめに選択されるものを指定する
    //1番目の「Excel」が選択されているようにする
    ofd.FilterIndex = 1;

    //タイトルを設定する
    ofd.Title = "秀丸へと読み込むExcelファイルを選択してください";
    //ダイアログボックスを閉じる前に現在のディレクトリを復元するようにする
    ofd.RestoreDirectory = true;

    //ダイアログを表示する
    let DialogResult: System.Windows.Forms.DialogResult = lib.System.Windows.Forms.DialogResult;
    if (ofd.ShowDialog() == DialogResult.OK)
    {
        //OKボタンがクリックされたとき、選択されたファイル名を返す
        return ofd.FileName;
    }

    return null;
}

/**
 * HmExcelFileLoader を利用して指定のxlsをこねる
 */
class HmExcelFileLoaderController {

    // 指定のExcelのフルパス
    private xlsFileFullPath: string;
    // 指定のExcelのベース名
    private xlsFileBaseName: string;

    // システムが使う一時フォルダ
    private strTempFolder: string

    // セルのデータをカスタム加工する関数
    private cellNormalizeAlgorithm: (cell: string) => string;

    public constructor(xlsFileFullPath: string) {
        this.xlsFileFullPath = xlsFileFullPath;
        this.xlsFileBaseName = clr.System.IO.Path.GetFileNameWithoutExtension(this.xlsFileFullPath);
        this.strTempFolder = clr.System.IO.Path.GetTempPath();
        // デフォルトでは何も加工しないデータ
        this.cellNormalizeAlgorithm = (cell: string) => cell;
    }

    /**
     * セルのデータをカスタム加工する関数の設定
     */ 
    public OverrideCellNormalizeAlgorithm(func: (cell: string) => string) {
        this.cellNormalizeAlgorithm = func;
    }

    /**
     * コンバート。コンバート出来た各TSVファイル名のリストが返ってくる
     */ 
    public Convert(): string[] {
        let SheetDataList: LoadedSheetDataStruct[] = this.ConvertExcelFileToSheetDataStruct();

        // 結果のファイル名リスト
        let resultTsvFileNameList: string[] = [];

        for (var sdata of SheetDataList) {


            // 保存に使えそうなフルパスの生成
            let tsvFileName: string = this.GetTsvFileNameFromSheetData(sdata);

            // シートデータからTSV形式の文字列へ
            let tsvFileData: string = this.ConvertSheetDataToTsvData(sdata);

            // ファイルへと保存
            if (tsvFileData) {
                var sw: System.IO.StreamWriter = null;
                try {
                    var sw: System.IO.StreamWriter = new clr.System.IO.StreamWriter(tsvFileName);
                    sw.Write(tsvFileData);
                    sw.Flush();
                    sw.Close();
                    sw = null;

                    let message: string = clr.System.String.Format("{0} → {1}への保存に成功", this.xlsFileFullPath, tsvFileName);
                    PrintOutputPane(message);

                    resultTsvFileNameList.push(tsvFileName);
                } catch (err) {
                    PrintOutputPane(err.toString());
                    let message: string = clr.System.String.Format("{0} → {1}への保存に失敗", this.xlsFileFullPath, tsvFileName);
                    PrintOutputPane(message);
                } finally {
                    if (sw) {
                        sw.Close();
                        sw = null;
                    }
                }
            } else {
                let message: string = clr.System.String.Format("{0} → {1}は内容が読み取れません", this.xlsFileFullPath, tsvFileName);
                PrintOutputPane(message);
            }
        }
        return resultTsvFileNameList;
    }

    /**
     * エクセルのファイルを、扱いやすいシートデータ的なクラス型のリストへ
     */
    private ConvertExcelFileToSheetDataStruct(): LoadedSheetDataStruct[] {
        return clr.HmExcelFileLoader.LoadExcel(this.xlsFileFullPath);
    }

    /**
     * ブック名とシート名を元に、妥当なTSVファイルフルパスを生成する。
     * 「ファイル名としては使えない文字」がシート名に含まれている時などが考慮されている。
     * @param sdata
     */
    private GetTsvFileNameFromSheetData(sdata: LoadedSheetDataStruct): string {

        // シート名にダメ文字が入っていた場合には、こちらの「NormalizedSheetName」にも文字列が入っている
        let normalizedSheetName = sdata.NormalizedSheetName ? sdata.NormalizedSheetName : sdata.SheetName;

        // ブック名やシート名を可能な限り反映した一時ファイル名を作成
        let tsvFileName = clr.System.String.Format("{0}{1}_{2}.csv", this.strTempFolder, this.xlsFileBaseName, normalizedSheetName);

        return tsvFileName;
    }

    /**
     * １つのシートをTSVデータ文字列にする
     * @param sdata
     */
    private ConvertSheetDataToTsvData(sdata: LoadedSheetDataStruct): string {

        // 64K程度のテキストバッファーを予め確保
        var sb: System.Text.StringBuilder = new clr.System.Text.StringBuilder(64*1024);

        // 各行について
        for (let row of sdata.Cells) {
            // 行内の各カラムについて
            for (let cell of row) {
                let normalizedCellValue: string = this.cellNormalizeAlgorithm(cell);
                // データを出して
                sb.Append(normalizedCellValue);
                // タブ区切り
                sb.Append("\t");
            }
            // 行の最後に改行
            sb.Append("\n");
        }
        var result = sb.ToString();
        sb = null;
        return result;
    }
}


/**
 * 今回のtsvfilenameを秀丸マクロスコープへと蓄積
 * @param tsvfilename 
 * @param index
 */
function TransmitToHmMacroScope(tsvfilename: string, index: number): void {
    hm.Macro.Var["#TsvFileNameListIndex"] = index;
    hm.Macro.Var["$TsvFileName"] = tsvfilename;
    hm.Macro.Eval(f => {
        /*
            $TsvFileNameList[#TsvFileNameListIndex] = $TsvFileName;
        */
    });
}


/**
 * 秀丸用のアウトプットパネル。簡易
 */
function PrintOutputPane(message: string): void {
    hm.Macro.Var["$OutputPaneMessageText"] = message;
    hm.Macro.Eval(f => {
        /*
            #HMOP = loaddll(hidemarudir + @"\HmOutputPane.dll");
            #r = dllfunc(#HMOP, "Output", hidemaruhandle(0), $OutputPaneMessageText + "\r\n");
            freedll( #HMOP );
        */
    });
}

Main();

