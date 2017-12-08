/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the MIT License
 */

using System;
using System.IO;
using NPOI.HSSF.UserModel;
using NPOI.XSSF.UserModel;
using NPOI.SS.UserModel;
using System.Collections.Generic;

public class HmExcelFileLoader
{
    /// <summary>
    /// dll外からも見えている型。外にはこの型のListを返すこととなる。
    /// </summary>
    public class LoadedSheetDataStruct
    {
        public string SheetName;
        public string NormalizedSheetName;
        public List<List<string>> Cells = new List<List<string>>();
    }

    // 外へと返す型
    private static List<LoadedSheetDataStruct> SheetList = new List<LoadedSheetDataStruct>();

    // ブック読み
    static public void ReadBook(IWorkbook book)
    {
        int numberOfSheets = book.NumberOfSheets;

        for (int sheetIndex = 0; sheetIndex < numberOfSheets; ++sheetIndex)
        {
            ISheet sheet = book.GetSheetAt(sheetIndex);
            ReadSheet(sheet, sheetIndex);
        }
    }

    // シート１つ分
    private static void ReadSheet(ISheet sheet, int sheetIndex)
    {
        var loaded_sheet = new LoadedSheetDataStruct() { SheetName = sheet.SheetName };
        char[] invalidChars = System.IO.Path.GetInvalidFileNameChars();

        // ファイル名に出来ない文字が入っている
        if (sheet.SheetName.IndexOfAny(invalidChars) >= 0)
        {
            loaded_sheet.NormalizedSheetName = "NormalizedSheetName" + sheetIndex;
        }


        int lastRowNum = sheet.LastRowNum;

        for (int rowIndex = 0 /*sheet.FirstRowNum*/; rowIndex <= lastRowNum; ++rowIndex)
        {
            IRow row = sheet.GetRow(rowIndex);

            List<string> loaded_row = new List<string>();

            if (row == null)
            {
                loaded_sheet.Cells.Add(loaded_row);
                continue;
            }

            int lastCellNum = row.LastCellNum;
            for (int cellIndex = 0 /*row.FirstCellNum*/; cellIndex < lastCellNum; ++cellIndex)
            {
                ICell cell = row.GetCell(cellIndex, MissingCellPolicy.RETURN_NULL_AND_BLANK);
                if (cell == null)
                {
                    loaded_row.Add("");
                }
                else
                {
                    string strCell = GetCellValue(cell);
                    loaded_row.Add(strCell);
                }
            }

            loaded_sheet.Cells.Add(loaded_row);

        }

        SheetList.Add(loaded_sheet);
    }

    private static string GetCellValue(ICell cell)
    {
        string strCell = "";
        switch (cell.CellType)
        {
            // 文字列型
            case CellType.String:
                strCell = cell.StringCellValue;
                break;
            // 数値型（日付の場合もここに入る）
            case CellType.Numeric:
                // セルが日付情報が単なる数値かを判定
                if (DateUtil.IsCellDateFormatted(cell))
                {
                    // 日付型
                    // 本来はスタイルに合わせてフォーマットすべきだが、
                    // うまく表示できないケースが若干見られたので固定のフォーマットとして取得
                    strCell = cell.DateCellValue.ToString("yyyy/MM/dd HH:mm:ss");
                }
                else
                {
                    // 数値型
                    strCell = cell.NumericCellValue.ToString();
                }
                break;

            // bool型(文字列でTrueとか入れておけばbool型として扱われた)
            case CellType.Boolean:
                strCell = cell.BooleanCellValue.ToString();
                break;

            // 入力なし
            case CellType.Blank:
                strCell = cell.ToString();
                break;

            // 数式
            case CellType.Formula:
                // 下記で数式の文字列が取得される
                //cellStr = cell.CellFormula.ToString();

                // 数式の元となったセルの型を取得して同様の処理を行う
                // コメントは省略
                switch (cell.CachedFormulaResultType)
                {
                    case CellType.String:
                        strCell = cell.StringCellValue;
                        break;
                    case CellType.Numeric:

                        if (DateUtil.IsCellDateFormatted(cell))
                        {
                            strCell = cell.DateCellValue.ToString("yyyy/MM/dd HH:mm:ss");
                        }
                        else
                        {
                            strCell = cell.NumericCellValue.ToString();
                        }
                        break;
                    case CellType.Boolean:
                        strCell = cell.BooleanCellValue.ToString();
                        break;
                    case CellType.Blank:
                        break;
                    case CellType.Error:
                        strCell = cell.ErrorCellValue.ToString();
                        break;
                    case CellType.Unknown:
                        strCell = cell.ToString();
                        break;
                    default:
                        strCell = cell.ToString();
                        break;
                }
                break;

            // エラー
            case CellType.Error:
                strCell = cell.ErrorCellValue.ToString();
                break;

            // 型不明なセル
            case CellType.Unknown:
                strCell = cell.ToString();
                break;
            // もっと不明なセル
            default:
                strCell = cell.ToString();
                break;

        }

        return strCell;
    }

    // Excel独自
    private static void ReadXLS(string path)
    {
        try
        {
            using (FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                ReadBook(new HSSFWorkbook(fs));
            }
        }
        catch
        {
            throw;
        }
    }

    // Excelオープンなほう
    private static void ReadXLSX(string path)
    {
        try
        {
            using (FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
            {
                ReadBook(new XSSFWorkbook(fs));
            }

        }
        catch
        {
            throw;
        }
    }

    // 外から見える関数。Excelのフルパス指定して、扱いやすい固有のデータ型にして返す。
    public static List<LoadedSheetDataStruct> LoadExcel(string strExcelFileFullPath)
    {
        SheetList.Clear();

        try
        {

            if (strExcelFileFullPath.EndsWith(".xls"))
            {
                ReadXLS(strExcelFileFullPath);
            }
            else if (strExcelFileFullPath.EndsWith(".xlsx"))
            {
                ReadXLSX(strExcelFileFullPath);
            }

        }
        catch (Exception e)
        {
            System.Windows.Forms.MessageBox.Show(e.Message);
        }

        return SheetList;
    }
}
