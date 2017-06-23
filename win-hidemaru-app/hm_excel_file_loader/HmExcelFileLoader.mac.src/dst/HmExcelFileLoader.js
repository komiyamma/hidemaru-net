///<reference path="HmV8.d.ts"/>
///<reference path="HmExcelFIleLoader.d.ts"/>
// このマクロファイルの場所をアセンブリの置き場としても登録する
let currentmacrodirectory = hm.Macro.Var["currentmacrodirectory"];
AssemblyPath.Add(currentmacrodirectory);
// アセンブリの読み込み
host.lib(clr, "HmExcelFileLoader");
let xlsFileName = "C:/abcc/test.xls";
let tsvFileNameList = ConvertWorkbookToSheetTsvList(xlsFileName);
for (let i = 0; i < tsvFileNameList.length; i++) {
    TransmissitToHidemaruVar(tsvFileNameList[i], i);
}
/**
 * エクセルファイル形式をロード(.xls, .xlsxの両方に対応している)
 * @param xlsFileName
 */
function ConvertWorkbookToSheetTsvList(xlsFileName) {
    // 元のExcelのファイル名のベース部
    let xlsBaseName = clr.System.IO.Path.GetFileNameWithoutExtension(xlsFileName);
    // システムが使う一時フォルダ
    let strTempFolder = clr.System.IO.Path.GetTempPath();
    // WorkBookの読み取り
    let LoadDatas = clr.HmExcelFileLoader.LoadExcel(xlsFileName);
    let tsvFileNameList = [];
    for (let sheet of LoadDatas) {
        let tmpFileName = "";
        // シート名にダメ文字が入っていた場合には、こちらのノーマライズシート名に文字列が入っている
        if (sheet.NormalizedSheetName) {
            tmpFileName = clr.System.String.Format("{0}{1}_{2}.csv", strTempFolder, xlsBaseName, sheet.NormalizedSheetName);
        }
        else {
            tmpFileName = clr.System.String.Format("{0}{1}_{2}.csv", strTempFolder, xlsBaseName, sheet.SheetName);
        }
        let canWrite = WriteSheetToTsv(tmpFileName, sheet);
        if (canWrite) {
            tsvFileNameList.push(tmpFileName);
        }
        else {
            let message = clr.System.String.Format("{0} → {1}への保存に失敗", xlsFileName, tmpFileName);
            PrintOutputPane(message);
        }
    }
    return tsvFileNameList;
}
/**
 * // １つのセルの値が、秀丸にとって不都合があるのであれば、変更する。
 * @param cellValue
 */
function NormalizeCellValue(cellValue) {
    let normalizedCellValue = cellValue.replace(/\n/g, "[[改行]]");
    return normalizedCellValue;
}
/**
 * １つのシートをTSVファイルへと出力する
 * @param tsvFileName
 * @param sheet
 */
function WriteSheetToTsv(tsvFileName, sheet) {
    try {
        var sw = new clr.System.IO.StreamWriter(tsvFileName);
        if (!sw) {
            return false;
        }
        // 各行について
        for (let row of sheet.Cells) {
            // 行内の各カラムについて
            for (let cell of row) {
                let normalizedCellValue = NormalizeCellValue(cell);
                // データを出して
                sw.Write(normalizedCellValue);
                // タブ区切り
                sw.Write("\t");
            }
            // 行の最後に改行
            sw.Write("\n");
        }
        sw.Flush();
        sw.Close();
        sw = null;
        return true;
    }
    catch (err) {
    }
    return false;
}
/**
 * 今回のtsvfilenameを秀丸マクロスコープへと伝搬
 * @param filename
 */
function TransmissitToHidemaruVar(tsvfilename, index) {
    // tslint:disable-next-line:no-string-literal
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
