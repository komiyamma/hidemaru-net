/**
 * WinAssemblyToTypeScriptDeclare.exe
 *  any IWorkbook any ICell any IRow any ISheet
 *  System.IO StringWriter System.IO StreamWriter System.Text StringBuilder
 *  System.Windows.Forms OpenFileDialog System.Windows.Forms DialogResult
 *  any LoadedSheetDataStruct any HmExcelFileLoader > a.txt
 * による自動生成
 */

/**
名前:HmExcelFileLoader
名前空間:無し
完全限定名:HmExcelFileLoader
このメンバを宣言するクラス:
親クラス:System.Object
属性:AutoLayout, AnsiClass, Class, Public, BeforeFieldInit
*/
declare interface HmExcelFileLoader {
    new();
    ReadBook(book: NPOI.SS.UserModel.IWorkbook): void;
    LoadExcel(strExcelFileFullPath: string): any;
    ToString(): string;
    Equals(obj: any): boolean;
    GetHashCode(): number;
    GetType(): System.Type;
}
/**
名前:LoadedSheetDataStruct
名前空間:無し
完全限定名:HmExcelFileLoader+LoadedSheetDataStruct
このメンバを宣言するクラス:HmExcelFileLoader
親クラス:System.Object
属性:AutoLayout, AnsiClass, Class, NestedPublic, BeforeFieldInit
*/
declare interface LoadedSheetDataStruct {
    new();
    SheetName :string;
    NormalizedSheetName :string;
    Cells :any;
    ToString(): string;
    Equals(obj: any): boolean;
    GetHashCode(): number;
    GetType(): System.Type;
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:ICell
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.ICell
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ICell {
                readonly ColumnIndex :number;
                readonly RowIndex :number;
                readonly Sheet :NPOI.SS.UserModel.ISheet;
                readonly Row :NPOI.SS.UserModel.IRow;
                readonly CellType :NPOI.SS.UserModel.CellType;
                readonly CachedFormulaResultType :NPOI.SS.UserModel.CellType;
                CellFormula :string;
                readonly NumericCellValue :number;
                readonly DateCellValue :System.DateTime;
                readonly RichStringCellValue :NPOI.SS.UserModel.IRichTextString;
                readonly ErrorCellValue :System.Byte;
                readonly StringCellValue :string;
                readonly BooleanCellValue :boolean;
                CellStyle :NPOI.SS.UserModel.ICellStyle;
                CellComment :NPOI.SS.UserModel.IComment;
                Hyperlink :NPOI.SS.UserModel.IHyperlink;
                readonly ArrayFormulaRange :NPOI.SS.Util.CellRangeAddress;
                readonly IsPartOfArrayFormulaGroup :boolean;
                readonly IsMergedCell :boolean;
                SetCellType(cellType: NPOI.SS.UserModel.CellType): void;
                SetCellValue(value: number): void;
                SetCellErrorValue(value: System.Byte): void;
                SetCellValue(value: System.DateTime): void;
                SetCellValue(value: NPOI.SS.UserModel.IRichTextString): void;
                SetCellValue(value: string): void;
                CopyCellTo(targetIndex: number): NPOI.SS.UserModel.ICell;
                SetCellFormula(formula: string): void;
                SetCellValue(value: boolean): void;
                SetAsActiveCell(): void;
                RemoveCellComment(): void;
                RemoveHyperlink(): void;
                GetCachedFormulaResultTypeEnum(): NPOI.SS.UserModel.CellType;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IRow
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IRow
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IRow {
                RowNum :number;
                readonly FirstCellNum :System.Int16;
                readonly LastCellNum :System.Int16;
                readonly PhysicalNumberOfCells :number;
                ZeroHeight :boolean;
                Height :System.Int16;
                HeightInPoints :number;
                readonly IsFormatted :boolean;
                readonly Sheet :NPOI.SS.UserModel.ISheet;
                RowStyle :NPOI.SS.UserModel.ICellStyle;
                readonly Cells :any;
                readonly OutlineLevel :number;
                Hidden :any;
                Collapsed :any;
                CreateCell(column: number): NPOI.SS.UserModel.ICell;
                CreateCell(column: number, type: NPOI.SS.UserModel.CellType): NPOI.SS.UserModel.ICell;
                RemoveCell(cell: NPOI.SS.UserModel.ICell): void;
                GetCell(cellnum: number): NPOI.SS.UserModel.ICell;
                GetCell(cellnum: number, policy: NPOI.SS.UserModel.MissingCellPolicy): NPOI.SS.UserModel.ICell;
                MoveCell(cell: NPOI.SS.UserModel.ICell, newColumn: number): void;
                CopyRowTo(targetIndex: number): NPOI.SS.UserModel.IRow;
                CopyCell(sourceIndex: number, targetIndex: number): NPOI.SS.UserModel.ICell;
                HasCustomHeight(): boolean;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:ISheet
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.ISheet
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ISheet {
                readonly PhysicalNumberOfRows :number;
                readonly FirstRowNum :number;
                readonly LastRowNum :number;
                ForceFormulaRecalculation :boolean;
                DefaultColumnWidth :number;
                DefaultRowHeight :System.Int16;
                DefaultRowHeightInPoints :number;
                HorizontallyCenter :boolean;
                VerticallyCenter :boolean;
                readonly NumMergedRegions :number;
                DisplayZeros :boolean;
                Autobreaks :boolean;
                DisplayGuts :boolean;
                FitToPage :boolean;
                RowSumsBelow :boolean;
                RowSumsRight :boolean;
                IsPrintGridlines :boolean;
                readonly PrintSetup :NPOI.SS.UserModel.IPrintSetup;
                readonly Header :NPOI.SS.UserModel.IHeader;
                readonly Footer :NPOI.SS.UserModel.IFooter;
                readonly Protect :boolean;
                readonly ScenarioProtect :boolean;
                TabColorIndex :System.Int16;
                readonly DrawingPatriarch :NPOI.SS.UserModel.IDrawing;
                TopRow :System.Int16;
                LeftCol :System.Int16;
                readonly PaneInformation :NPOI.SS.Util.PaneInformation;
                DisplayGridlines :boolean;
                DisplayFormulas :boolean;
                DisplayRowColHeadings :boolean;
                IsActive :boolean;
                readonly RowBreaks :number[];
                readonly ColumnBreaks :number[];
                readonly Workbook :NPOI.SS.UserModel.IWorkbook;
                readonly SheetName :string;
                IsSelected :boolean;
                readonly SheetConditionalFormatting :NPOI.SS.UserModel.ISheetConditionalFormatting;
                IsRightToLeft :boolean;
                RepeatingRows :NPOI.SS.Util.CellRangeAddress;
                RepeatingColumns :NPOI.SS.Util.CellRangeAddress;
                CreateRow(rownum: number): NPOI.SS.UserModel.IRow;
                RemoveRow(row: NPOI.SS.UserModel.IRow): void;
                GetRow(rownum: number): NPOI.SS.UserModel.IRow;
                SetColumnHidden(columnIndex: number, hidden: boolean): void;
                IsColumnHidden(columnIndex: number): boolean;
                CopyRow(sourceIndex: number, targetIndex: number): NPOI.SS.UserModel.IRow;
                SetColumnWidth(columnIndex: number, width: number): void;
                GetColumnWidth(columnIndex: number): number;
                GetColumnWidthInPixels(columnIndex: number): number;
                GetColumnStyle(column: number): NPOI.SS.UserModel.ICellStyle;
                AddMergedRegion(region: NPOI.SS.Util.CellRangeAddress): number;
                RemoveMergedRegion(index: number): void;
                GetMergedRegion(index: number): NPOI.SS.Util.CellRangeAddress;
                GetRowEnumerator(): System.Collections.IEnumerator;
                GetEnumerator(): System.Collections.IEnumerator;
                [Symbol.iterator](): IterableIterator<any>;
                GetMargin(margin: NPOI.SS.UserModel.MarginType): number;
                SetMargin(margin: NPOI.SS.UserModel.MarginType, size: number): void;
                ProtectSheet(password: string): void;
                SetZoom(numerator: number, denominator: number): void;
                ShowInPane(toprow: number, leftcol: number): void;
                ShowInPane(toprow: System.Int16, leftcol: System.Int16): void;
                ShiftRows(startRow: number, endRow: number, n: number): void;
                ShiftRows(startRow: number, endRow: number, n: number, copyRowHeight: boolean, resetOriginalRowHeight: boolean): void;
                CreateFreezePane(colSplit: number, rowSplit: number, leftmostColumn: number, topRow: number): void;
                CreateFreezePane(colSplit: number, rowSplit: number): void;
                CreateSplitPane(xSplitPos: number, ySplitPos: number, leftmostColumn: number, topRow: number, activePane: NPOI.SS.UserModel.PanePosition): void;
                IsRowBroken(row: number): boolean;
                RemoveRowBreak(row: number): void;
                SetActiveCell(row: number, column: number): void;
                SetActiveCellRange(firstRow: number, lastRow: number, firstColumn: number, lastColumn: number): void;
                SetActiveCellRange<D>(cellranges: any, activeRange: number, activeRow: number, activeColumn: number): void;
                SetColumnBreak(column: number): void;
                SetRowBreak(row: number): void;
                IsColumnBroken(column: number): boolean;
                RemoveColumnBreak(column: number): void;
                SetColumnGroupCollapsed(columnNumber: number, collapsed: boolean): void;
                GroupColumn(fromColumn: number, toColumn: number): void;
                UngroupColumn(fromColumn: number, toColumn: number): void;
                GroupRow(fromRow: number, toRow: number): void;
                UngroupRow(fromRow: number, toRow: number): void;
                SetRowGroupCollapsed(row: number, collapse: boolean): void;
                SetDefaultColumnStyle(column: number, style: NPOI.SS.UserModel.ICellStyle): void;
                AutoSizeColumn(column: number): void;
                AutoSizeColumn(column: number, useMergedCells: boolean): void;
                GetCellComment(row: number, column: number): NPOI.SS.UserModel.IComment;
                CreateDrawingPatriarch(): NPOI.SS.UserModel.IDrawing;
                SetActive(value: boolean): void;
                SetArrayFormula(formula: string, range: NPOI.SS.Util.CellRangeAddress): any;
                RemoveArrayFormula(cell: NPOI.SS.UserModel.ICell): any;
                IsMergedRegion(mergedRegion: NPOI.SS.Util.CellRangeAddress): boolean;
                GetDataValidationHelper(): NPOI.SS.UserModel.IDataValidationHelper;
                GetDataValidations(): any;
                AddValidationData(dataValidation: NPOI.SS.UserModel.IDataValidation): void;
                SetAutoFilter(range: NPOI.SS.Util.CellRangeAddress): NPOI.SS.UserModel.IAutoFilter;
                CopySheet(Name: string): NPOI.SS.UserModel.ISheet;
                CopySheet(Name: string, copyStyle: boolean): NPOI.SS.UserModel.ISheet;
                GetColumnOutlineLevel(columnIndex: number): number;
                IsDate1904(): boolean;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IWorkbook
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IWorkbook
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IWorkbook {
                readonly ActiveSheetIndex :number;
                FirstVisibleTab :number;
                readonly NumberOfSheets :number;
                readonly NumberOfFonts :System.Int16;
                readonly NumCellStyles :System.Int16;
                readonly NumberOfNames :number;
                MissingCellPolicy :NPOI.SS.UserModel.MissingCellPolicy;
                IsHidden :boolean;
                SetSheetOrder(sheetname: string, pos: number): void;
                SetSelectedTab(index: number): void;
                SetActiveSheet(sheetIndex: number): void;
                GetSheetName(sheet: number): string;
                SetSheetName(sheet: number, name: string): void;
                GetSheetIndex(name: string): number;
                GetSheetIndex(sheet: NPOI.SS.UserModel.ISheet): number;
                CreateSheet(): NPOI.SS.UserModel.ISheet;
                CreateSheet(sheetname: string): NPOI.SS.UserModel.ISheet;
                CloneSheet(sheetNum: number): NPOI.SS.UserModel.ISheet;
                GetSheetAt(index: number): NPOI.SS.UserModel.ISheet;
                GetSheet(name: string): NPOI.SS.UserModel.ISheet;
                RemoveSheetAt(index: number): void;
                GetEnumerator(): System.Collections.IEnumerator;
                [Symbol.iterator](): IterableIterator<any>;
                SetRepeatingRowsAndColumns(sheetIndex: number, startColumn: number, endColumn: number, startRow: number, endRow: number): void;
                CreateFont(): NPOI.SS.UserModel.IFont;
                FindFont(boldWeight: System.Int16, color: System.Int16, fontHeight: System.Int16, name: string, italic: boolean, strikeout: boolean, typeOffset: NPOI.SS.UserModel.FontSuperScript, underline: NPOI.SS.UserModel.FontUnderlineType): NPOI.SS.UserModel.IFont;
                GetFontAt(idx: System.Int16): NPOI.SS.UserModel.IFont;
                CreateCellStyle(): NPOI.SS.UserModel.ICellStyle;
                GetCellStyleAt(idx: System.Int16): NPOI.SS.UserModel.ICellStyle;
                Write(stream: System.IO.Stream): void;
                GetName(name: string): NPOI.SS.UserModel.IName;
                GetNameAt(nameIndex: number): NPOI.SS.UserModel.IName;
                CreateName(): NPOI.SS.UserModel.IName;
                GetNameIndex(name: string): number;
                RemoveName(index: number): void;
                RemoveName(name: string): void;
                LinkExternalWorkbook(name: string, workbook: NPOI.SS.UserModel.IWorkbook): number;
                SetPrintArea(sheetIndex: number, reference: string): void;
                SetPrintArea(sheetIndex: number, startColumn: number, endColumn: number, startRow: number, endRow: number): void;
                GetPrintArea(sheetIndex: number): string;
                RemovePrintArea(sheetIndex: number): void;
                CreateDataFormat(): NPOI.SS.UserModel.IDataFormat;
                AddPicture(pictureData: System.Byte[], format: NPOI.SS.UserModel.PictureType): number;
                GetAllPictures(): System.Collections.IList;
                GetCreationHelper(): NPOI.SS.UserModel.ICreationHelper;
                IsSheetHidden(sheetIx: number): boolean;
                IsSheetVeryHidden(sheetIx: number): boolean;
                SetSheetHidden(sheetIx: number, hidden: NPOI.SS.UserModel.SheetState): void;
                SetSheetHidden(sheetIx: number, hidden: number): void;
                AddToolPack(toopack: NPOI.SS.Formula.Udf.UDFFinder): void;
                Close(): void;
                Dispose(): boolean;
            }
        }
    }
}
declare namespace NPOI {
    namespace XWPF {
        namespace UserModel {
            /**
            名前:ICell
            名前空間:NPOI.XWPF.UserModel
            完全限定名:NPOI.XWPF.UserModel.ICell
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ICell {
            }
        }
    }
}
declare namespace System {
    namespace IO {
        /**
        名前:Path
        名前空間:System.IO
        完全限定名:System.IO.Path
        このメンバを宣言するクラス:
        親クラス:System.Object
        属性:AutoLayout, AnsiClass, Class, Public, Abstract, Sealed, BeforeFieldInit
        */
        interface Path {
            readonly DirectorySeparatorChar :System.Char;
            readonly AltDirectorySeparatorChar :System.Char;
            readonly VolumeSeparatorChar :System.Char;
            readonly InvalidPathChars :System.Char[];
            readonly PathSeparator :System.Char;
            ChangeExtension(path: string, extension: string): string;
            GetDirectoryName(path: string): string;
            GetInvalidPathChars(): System.Char[];
            GetInvalidFileNameChars(): System.Char[];
            GetExtension(path: string): string;
            GetFullPath(path: string): string;
            GetFileName(path: string): string;
            GetFileNameWithoutExtension(path: string): string;
            GetPathRoot(path: string): string;
            GetTempPath(): string;
            GetRandomFileName(): string;
            GetTempFileName(): string;
            HasExtension(path: string): boolean;
            IsPathRooted(path: string): boolean;
            Combine(path1: string, path2: string): string;
            Combine(path1: string, path2: string, path3: string): string;
            Combine(path1: string, path2: string, path3: string, path4: string): string;
            Combine(...paths: string[]): string;
            ToString(): string;
            Equals(obj: any): boolean;
            GetHashCode(): number;
            GetType(): System.Type;
        }
    }
}
declare namespace System {
    namespace IO {
        /**
        名前:StreamWriter
        名前空間:System.IO
        完全限定名:System.IO.StreamWriter
        このメンバを宣言するクラス:
        親クラス:System.IO.TextWriter
        属性:AutoLayout, AnsiClass, Class, Public, Serializable, BeforeFieldInit
        */
        interface StreamWriter {
            new(stream: System.IO.Stream);
            new(stream: System.IO.Stream, encoding: System.Text.Encoding);
            new(stream: System.IO.Stream, encoding: System.Text.Encoding, bufferSize: number, leaveOpen: boolean);
            new(path: string, append: boolean, encoding: System.Text.Encoding);
            new(stream: System.IO.Stream, encoding: System.Text.Encoding, bufferSize: number);
            new(path: string);
            new(path: string, append: boolean);
            new(path: string, append: boolean, encoding: System.Text.Encoding, bufferSize: number);
            readonly Null :System.IO.StreamWriter;
            AutoFlush :boolean;
            readonly BaseStream :System.IO.Stream;
            readonly Encoding :System.Text.Encoding;
            readonly FormatProvider :System.IFormatProvider;
            NewLine :string;
            Write(buffer: System.Char[], index: number, count: number): void;
            Close(): void;
            Flush(): void;
            Write(value: System.Char): void;
            Write(buffer: System.Char[]): void;
            Write(value: string): void;
            WriteAsync(value: System.Char): System.Threading.Tasks.Task;
            WriteAsync(value: string): System.Threading.Tasks.Task;
            WriteAsync(buffer: System.Char[], index: number, count: number): System.Threading.Tasks.Task;
            WriteLineAsync(): System.Threading.Tasks.Task;
            WriteLineAsync(value: System.Char): System.Threading.Tasks.Task;
            WriteLineAsync(value: string): System.Threading.Tasks.Task;
            WriteLineAsync(buffer: System.Char[], index: number, count: number): System.Threading.Tasks.Task;
            FlushAsync(): System.Threading.Tasks.Task;
            Dispose(): void;
            Write(value: boolean): void;
            Write(value: number): void;
            Write(value: any): void;
            Write(format: string, arg0: any): void;
            Write(format: string, arg0: any, arg1: any): void;
            Write(format: string, arg0: any, arg1: any, arg2: any): void;
            Write(format: string, ...arg: any[]): void;
            WriteLine(): void;
            WriteLine(value: System.Char): void;
            WriteLine(buffer: System.Char[]): void;
            WriteLine(buffer: System.Char[], index: number, count: number): void;
            WriteLine(value: boolean): void;
            WriteLine(value: number): void;
            WriteLine(value: string): void;
            WriteLine(value: any): void;
            WriteLine(format: string, arg0: any): void;
            WriteLine(format: string, arg0: any, arg1: any): void;
            WriteLine(format: string, arg0: any, arg1: any, arg2: any): void;
            WriteLine(format: string, ...arg: any[]): void;
            WriteAsync(buffer: System.Char[]): System.Threading.Tasks.Task;
            WriteLineAsync(buffer: System.Char[]): System.Threading.Tasks.Task;
            GetLifetimeService(): any;
            InitializeLifetimeService(): any;
            CreateObjRef(requestedType: System.Type): System.Runtime.Remoting.ObjRef;
            ToString(): string;
            Equals(obj: any): boolean;
            GetHashCode(): number;
            GetType(): System.Type;
        }
    }
}
declare namespace System {
    namespace IO {
        /**
        名前:StringWriter
        名前空間:System.IO
        完全限定名:System.IO.StringWriter
        このメンバを宣言するクラス:
        親クラス:System.IO.TextWriter
        属性:AutoLayout, AnsiClass, Class, Public, Serializable, BeforeFieldInit
        */
        interface StringWriter {
            new();
            new(formatProvider: System.IFormatProvider);
            new(sb: System.Text.StringBuilder);
            new(sb: System.Text.StringBuilder, formatProvider: System.IFormatProvider);
            readonly Encoding :System.Text.Encoding;
            readonly FormatProvider :System.IFormatProvider;
            NewLine :string;
            Close(): void;
            GetStringBuilder(): System.Text.StringBuilder;
            Write(value: System.Char): void;
            Write(buffer: System.Char[], index: number, count: number): void;
            Write(value: string): void;
            WriteAsync(value: System.Char): System.Threading.Tasks.Task;
            WriteAsync(value: string): System.Threading.Tasks.Task;
            WriteAsync(buffer: System.Char[], index: number, count: number): System.Threading.Tasks.Task;
            WriteLineAsync(value: System.Char): System.Threading.Tasks.Task;
            WriteLineAsync(value: string): System.Threading.Tasks.Task;
            WriteLineAsync(buffer: System.Char[], index: number, count: number): System.Threading.Tasks.Task;
            FlushAsync(): System.Threading.Tasks.Task;
            ToString(): string;
            Dispose(): void;
            Flush(): void;
            Write(buffer: System.Char[]): void;
            Write(value: boolean): void;
            Write(value: number): void;
            Write(value: any): void;
            Write(format: string, arg0: any): void;
            Write(format: string, arg0: any, arg1: any): void;
            Write(format: string, arg0: any, arg1: any, arg2: any): void;
            Write(format: string, ...arg: any[]): void;
            WriteLine(): void;
            WriteLine(value: System.Char): void;
            WriteLine(buffer: System.Char[]): void;
            WriteLine(buffer: System.Char[], index: number, count: number): void;
            WriteLine(value: boolean): void;
            WriteLine(value: number): void;
            WriteLine(value: string): void;
            WriteLine(value: any): void;
            WriteLine(format: string, arg0: any): void;
            WriteLine(format: string, arg0: any, arg1: any): void;
            WriteLine(format: string, arg0: any, arg1: any, arg2: any): void;
            WriteLine(format: string, ...arg: any[]): void;
            WriteAsync(buffer: System.Char[]): System.Threading.Tasks.Task;
            WriteLineAsync(buffer: System.Char[]): System.Threading.Tasks.Task;
            WriteLineAsync(): System.Threading.Tasks.Task;
            GetLifetimeService(): any;
            InitializeLifetimeService(): any;
            CreateObjRef(requestedType: System.Type): System.Runtime.Remoting.ObjRef;
            Equals(obj: any): boolean;
            GetHashCode(): number;
            GetType(): System.Type;
        }
    }
}
declare namespace System {
    namespace Text {
        /**
        名前:StringBuilder
        名前空間:System.Text
        完全限定名:System.Text.StringBuilder
        このメンバを宣言するクラス:
        親クラス:System.Object
        属性:AutoLayout, AnsiClass, Class, Public, Sealed, Serializable, BeforeFieldInit
        */
        interface StringBuilder {
            new();
            new(capacity: number);
            new(value: string);
            new(value: string, capacity: number);
            new(value: string, startIndex: number, length: number, capacity: number);
            new(capacity: number, maxCapacity: number);
            Capacity :number;
            readonly MaxCapacity :number;
            Length :number;
            Chars :System.Char;
            EnsureCapacity(capacity: number): number;
            ToString(): string;
            ToString(startIndex: number, length: number): string;
            Clear(): System.Text.StringBuilder;
            Append(value: System.Char, repeatCount: number): System.Text.StringBuilder;
            Append(value: System.Char[], startIndex: number, charCount: number): System.Text.StringBuilder;
            Append(value: string): System.Text.StringBuilder;
            Append(value: string, startIndex: number, count: number): System.Text.StringBuilder;
            AppendLine(): System.Text.StringBuilder;
            AppendLine(value: string): System.Text.StringBuilder;
            CopyTo(sourceIndex: number, destination: System.Char[], destinationIndex: number, count: number): void;
            Insert(index: number, value: string, count: number): System.Text.StringBuilder;
            Remove(startIndex: number, length: number): System.Text.StringBuilder;
            Append(value: boolean): System.Text.StringBuilder;
            Append(value: System.SByte): System.Text.StringBuilder;
            Append(value: System.Byte): System.Text.StringBuilder;
            Append(value: System.Char): System.Text.StringBuilder;
            Append(value: System.Int16): System.Text.StringBuilder;
            Append(value: number): System.Text.StringBuilder;
            Append(value: System.UInt16): System.Text.StringBuilder;
            Append(value: any): System.Text.StringBuilder;
            Append(value: System.Char[]): System.Text.StringBuilder;
            Insert(index: number, value: string): System.Text.StringBuilder;
            Insert(index: number, value: boolean): System.Text.StringBuilder;
            Insert(index: number, value: System.SByte): System.Text.StringBuilder;
            Insert(index: number, value: System.Byte): System.Text.StringBuilder;
            Insert(index: number, value: System.Int16): System.Text.StringBuilder;
            Insert(index: number, value: System.Char): System.Text.StringBuilder;
            Insert(index: number, value: System.Char[]): System.Text.StringBuilder;
            Insert(index: number, value: System.Char[], startIndex: number, charCount: number): System.Text.StringBuilder;
            Insert(index: number, value: number): System.Text.StringBuilder;
            Insert(index: number, value: System.UInt16): System.Text.StringBuilder;
            Insert(index: number, value: any): System.Text.StringBuilder;
            AppendFormat(format: string, arg0: any): System.Text.StringBuilder;
            AppendFormat(format: string, arg0: any, arg1: any): System.Text.StringBuilder;
            AppendFormat(format: string, arg0: any, arg1: any, arg2: any): System.Text.StringBuilder;
            AppendFormat(format: string, ...args: any[]): System.Text.StringBuilder;
            AppendFormat(provider: System.IFormatProvider, format: string, arg0: any): System.Text.StringBuilder;
            AppendFormat(provider: System.IFormatProvider, format: string, arg0: any, arg1: any): System.Text.StringBuilder;
            AppendFormat(provider: System.IFormatProvider, format: string, arg0: any, arg1: any, arg2: any): System.Text.StringBuilder;
            AppendFormat(provider: System.IFormatProvider, format: string, ...args: any[]): System.Text.StringBuilder;
            Replace(oldValue: string, newValue: string): System.Text.StringBuilder;
            Equals(sb: System.Text.StringBuilder): boolean;
            Replace(oldValue: string, newValue: string, startIndex: number, count: number): System.Text.StringBuilder;
            Replace(oldChar: System.Char, newChar: System.Char): System.Text.StringBuilder;
            Replace(oldChar: System.Char, newChar: System.Char, startIndex: number, count: number): System.Text.StringBuilder;
            Append(value: System.Char, valueCount: number): System.Text.StringBuilder;
            Equals(obj: any): boolean;
            GetHashCode(): number;
            GetType(): System.Type;
        }
    }
}
declare namespace System {
    namespace Windows {
        namespace Forms {
            /**
            名前:DialogResult
            名前空間:System.Windows.Forms
            完全限定名:System.Windows.Forms.DialogResult
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface DialogResult {
                value__ :number;
                readonly None :System.Windows.Forms.DialogResult;
                readonly OK :System.Windows.Forms.DialogResult;
                readonly Cancel :System.Windows.Forms.DialogResult;
                readonly Abort :System.Windows.Forms.DialogResult;
                readonly Retry :System.Windows.Forms.DialogResult;
                readonly Ignore :System.Windows.Forms.DialogResult;
                readonly Yes :System.Windows.Forms.DialogResult;
                readonly No :System.Windows.Forms.DialogResult;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: System.IFormatProvider): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: System.IFormatProvider): string;
                HasFlag(flag: System.Enum): boolean;
                GetTypeCode(): System.TypeCode;
                GetType(): System.Type;
            }
        }
    }
}
declare namespace System {
    namespace Windows {
        namespace Forms {
            /**
            名前:OpenFileDialog
            名前空間:System.Windows.Forms
            完全限定名:System.Windows.Forms.OpenFileDialog
            このメンバを宣言するクラス:
            親クラス:System.Windows.Forms.FileDialog
            属性:AutoLayout, AnsiClass, Class, Public, Sealed, BeforeFieldInit
            */
            interface OpenFileDialog {
                new();
                CheckFileExists :boolean;
                Multiselect :boolean;
                ReadOnlyChecked :boolean;
                ShowReadOnly :boolean;
                readonly SafeFileName :string;
                readonly SafeFileNames :string[];
                AddExtension :boolean;
                CheckPathExists :boolean;
                DefaultExt :string;
                DereferenceLinks :boolean;
                FileName :string;
                readonly FileNames :string[];
                Filter :string;
                FilterIndex :number;
                InitialDirectory :string;
                RestoreDirectory :boolean;
                ShowHelp :boolean;
                SupportMultiDottedExtensions :boolean;
                Title :string;
                ValidateNames :boolean;
                readonly CustomPlaces :System.Windows.Forms.FileDialogCustomPlacesCollection;
                AutoUpgradeEnabled :boolean;
                Tag :any;
                Site :System.ComponentModel.ISite;
                readonly Container :System.ComponentModel.IContainer;
                FileOk :System.ComponentModel.CancelEventHandler;
                HelpRequest :System.EventHandler;
                Disposed :System.EventHandler;
                OpenFile(): System.IO.Stream;
                Reset(): void;
                ToString(): string;
                ShowDialog(): System.Windows.Forms.DialogResult;
                ShowDialog(owner: System.Windows.Forms.IWin32Window): System.Windows.Forms.DialogResult;
                Dispose(): void;
                GetLifetimeService(): any;
                InitializeLifetimeService(): any;
                CreateObjRef(requestedType: System.Type): System.Runtime.Remoting.ObjRef;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): System.Type;
            }
        }
    }
}
declare namespace System {
    namespace Collections {
        namespace Generic {
            /**
            名前:List`1
            名前空間:System.Collections.Generic
            完全限定名:System.Collections.Generic.List`1
            このメンバを宣言するクラス:
            親クラス:System.Object
            属性:AutoLayout, AnsiClass, Class, Public, Serializable, BeforeFieldInit
            */
            interface List<T> {
                new();
                new(capacity: number);
                new(collection: any);
                Capacity :number;
                readonly Count :number;
                Item :any;
                [index: number]: any;
                Add(item: any): void;
                AddRange(collection: any): void;
                BinarySearch(index: number, count: number, item: any, comparer: any): number;
                Clear(): void;
                Contains(item: any): boolean;
                ConvertAll<TOutput>(converter: any): any;
                CopyTo(index: number, array: any, arrayIndex: number, count: number): void;
                ForEach(action: any): void;
                GetEnumerator(): any;
                [Symbol.iterator](): IterableIterator<any>;
                IndexOf(item: any): number;
                Insert(index: number, item: any): void;
                InsertRange(index: number, collection: any): void;
                Remove(item: any): boolean;
                RemoveAll(match: any): number;
                RemoveAt(index: number): void;
                RemoveRange(index: number, count: number): void;
                Reverse(index: number, count: number): void;
                Sort(): void;
                Sort(comparer: any): void;
                Sort(index: number, count: number, comparer: any): void;
                Sort(comparison: any): void;
                ToArray(): any;
                TrimExcess(): void;
                AsReadOnly(): any;
                BinarySearch(item: any): number;
                BinarySearch(item: any, comparer: any): number;
                CopyTo(array: any): void;
                CopyTo(array: any, arrayIndex: number): void;
                Exists(match: any): boolean;
                Find(match: any): any;
                FindAll(match: any): any;
                FindIndex(match: any): number;
                FindIndex(startIndex: number, match: any): number;
                FindIndex(startIndex: number, count: number, match: any): number;
                FindLast(match: any): any;
                FindLastIndex(match: any): number;
                FindLastIndex(startIndex: number, match: any): number;
                FindLastIndex(startIndex: number, count: number, match: any): number;
                GetRange(index: number, count: number): any;
                IndexOf(item: any, index: number): number;
                IndexOf(item: any, index: number, count: number): number;
                LastIndexOf(item: any): number;
                LastIndexOf(item: any, index: number): number;
                LastIndexOf(item: any, index: number, count: number): number;
                Reverse(): void;
                TrueForAll(match: any): boolean;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    /**
    名前:Type
    名前空間:System
    完全限定名:System.Type
    このメンバを宣言するクラス:
    親クラス:System.Reflection.MemberInfo
    属性:AutoLayout, AnsiClass, Class, Public, Abstract, Serializable, BeforeFieldInit
    */
    interface Type {
        readonly FilterAttribute :any;
        readonly FilterName :any;
        readonly FilterNameIgnoreCase :any;
        readonly Missing :any;
        readonly Delimiter :any;
        readonly EmptyTypes :any;
        readonly MemberType :any;
        readonly DeclaringType :any;
        readonly DeclaringMethod :any;
        readonly ReflectedType :any;
        readonly StructLayoutAttribute :any;
        readonly GUID :any;
        readonly DefaultBinder :any;
        readonly Module :any;
        readonly Assembly :any;
        readonly TypeHandle :any;
        readonly FullName :string;
        readonly Namespace :string;
        readonly AssemblyQualifiedName :string;
        readonly BaseType :any;
        readonly TypeInitializer :any;
        readonly IsNested :boolean;
        readonly Attributes :any;
        readonly GenericParameterAttributes :any;
        readonly IsVisible :boolean;
        readonly IsNotPublic :boolean;
        readonly IsPublic :boolean;
        readonly IsNestedPublic :boolean;
        readonly IsNestedPrivate :boolean;
        readonly IsNestedFamily :boolean;
        readonly IsNestedAssembly :boolean;
        readonly IsNestedFamANDAssem :boolean;
        readonly IsNestedFamORAssem :boolean;
        readonly IsAutoLayout :boolean;
        readonly IsLayoutSequential :boolean;
        readonly IsExplicitLayout :boolean;
        readonly IsClass :boolean;
        readonly IsInterface :boolean;
        readonly IsValueType :boolean;
        readonly IsAbstract :boolean;
        readonly IsSealed :boolean;
        readonly IsEnum :boolean;
        readonly IsSpecialName :boolean;
        readonly IsImport :boolean;
        readonly IsSerializable :boolean;
        readonly IsAnsiClass :boolean;
        readonly IsUnicodeClass :boolean;
        readonly IsAutoClass :boolean;
        readonly IsArray :boolean;
        readonly IsGenericType :boolean;
        readonly IsGenericTypeDefinition :boolean;
        readonly IsConstructedGenericType :boolean;
        readonly IsGenericParameter :boolean;
        readonly GenericParameterPosition :number;
        readonly ContainsGenericParameters :boolean;
        readonly IsByRef :boolean;
        readonly IsPointer :boolean;
        readonly IsPrimitive :boolean;
        readonly IsCOMObject :boolean;
        readonly HasElementType :boolean;
        readonly IsContextful :boolean;
        readonly IsMarshalByRef :boolean;
        readonly GenericTypeArguments :any;
        readonly IsSecurityCritical :boolean;
        readonly IsSecuritySafeCritical :boolean;
        readonly IsSecurityTransparent :boolean;
        readonly UnderlyingSystemType :any;
        readonly Name :string;
        readonly CustomAttributes :any;
        readonly MetadataToken :number;
        GetType(typeName: string, throwOnError: boolean, ignoreCase: boolean): any;
        GetType(typeName: string, throwOnError: boolean): any;
        GetType(typeName: string): any;
        GetType<D>(typeName: string, assemblyResolver: any, typeResolver: any): any;
        GetType<D>(typeName: string, assemblyResolver: any, typeResolver: any, throwOnError: boolean): any;
        GetType<D>(typeName: string, assemblyResolver: any, typeResolver: any, throwOnError: boolean, ignoreCase: boolean): any;
        ReflectionOnlyGetType(typeName: string, throwIfNotFound: boolean, ignoreCase: boolean): any;
        MakePointerType(): any;
        MakeByRefType(): any;
        MakeArrayType(): any;
        MakeArrayType(rank: number): any;
        GetTypeFromProgID(progID: string): any;
        GetTypeFromProgID(progID: string, throwOnError: boolean): any;
        GetTypeFromProgID(progID: string, server: string): any;
        GetTypeFromProgID(progID: string, server: string, throwOnError: boolean): any;
        GetTypeFromCLSID(clsid: any): any;
        GetTypeFromCLSID(clsid: any, throwOnError: boolean): any;
        GetTypeFromCLSID(clsid: any, server: string): any;
        GetTypeFromCLSID(clsid: any, server: string, throwOnError: boolean): any;
        GetTypeCode(type: any): any;
        InvokeMember(name: string, invokeAttr: any, binder: any, target: any, args: any, modifiers: any, culture: any, namedParameters: any): any;
        InvokeMember(name: string, invokeAttr: any, binder: any, target: any, args: any, culture: any): any;
        InvokeMember(name: string, invokeAttr: any, binder: any, target: any, args: any): any;
        GetTypeHandle(o: any): any;
        GetTypeFromHandle(handle: any): any;
        GetArrayRank(): number;
        GetConstructor(bindingAttr: any, binder: any, callConvention: any, types: any, modifiers: any): any;
        GetConstructor(bindingAttr: any, binder: any, types: any, modifiers: any): any;
        GetConstructor(types: any): any;
        GetConstructors(): any;
        GetConstructors(bindingAttr: any): any;
        GetMethod(name: string, bindingAttr: any, binder: any, callConvention: any, types: any, modifiers: any): any;
        GetMethod(name: string, bindingAttr: any, binder: any, types: any, modifiers: any): any;
        GetMethod(name: string, types: any, modifiers: any): any;
        GetMethod(name: string, types: any): any;
        GetMethod(name: string, bindingAttr: any): any;
        GetMethod(name: string): any;
        GetMethods(): any;
        GetMethods(bindingAttr: any): any;
        GetField(name: string, bindingAttr: any): any;
        GetField(name: string): any;
        GetFields(): any;
        GetFields(bindingAttr: any): any;
        GetInterface(name: string): any;
        GetInterface(name: string, ignoreCase: boolean): any;
        GetInterfaces(): any;
        FindInterfaces(filter: any, filterCriteria: any): any;
        GetEvent(name: string): any;
        GetEvent(name: string, bindingAttr: any): any;
        GetEvents(): any;
        GetEvents(bindingAttr: any): any;
        GetProperty(name: string, bindingAttr: any, binder: any, returnType: any, types: any, modifiers: any): any;
        GetProperty(name: string, returnType: any, types: any, modifiers: any): any;
        GetProperty(name: string, bindingAttr: any): any;
        GetProperty(name: string, returnType: any, types: any): any;
        GetProperty(name: string, types: any): any;
        GetProperty(name: string, returnType: any): any;
        GetProperty(name: string): any;
        GetProperties(bindingAttr: any): any;
        GetProperties(): any;
        GetNestedTypes(): any;
        GetNestedTypes(bindingAttr: any): any;
        GetNestedType(name: string): any;
        GetNestedType(name: string, bindingAttr: any): any;
        GetMember(name: string): any;
        GetMember(name: string, bindingAttr: any): any;
        GetMember(name: string, type: any, bindingAttr: any): any;
        GetMembers(): any;
        GetMembers(bindingAttr: any): any;
        GetDefaultMembers(): any;
        FindMembers(memberType: any, bindingAttr: any, filter: any, filterCriteria: any): any;
        GetGenericParameterConstraints(): any;
        MakeGenericType(...typeArguments: any[]): any;
        GetElementType(): any;
        GetGenericArguments(): any;
        GetGenericTypeDefinition(): any;
        GetEnumNames(): any;
        GetEnumValues(): any;
        GetEnumUnderlyingType(): any;
        IsEnumDefined(value: any): boolean;
        GetEnumName(value: any): string;
        IsSubclassOf(c: any): boolean;
        IsInstanceOfType(o: any): boolean;
        IsAssignableFrom(c: any): boolean;
        IsEquivalentTo(other: any): boolean;
        ToString(): string;
        GetTypeArray(args: any): any;
        Equals(o: any): boolean;
        GetHashCode(): number;
        GetInterfaceMap(interfaceType: any): any;
        GetType(): any;
        GetCustomAttributes(inherit: boolean): any;
        GetCustomAttributes(attributeType: any, inherit: boolean): any;
        IsDefined(attributeType: any, inherit: boolean): boolean;
        GetCustomAttributesData(): any;
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:CellType
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.CellType
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface CellType {
                value__ :number;
                readonly Unknown :any;
                readonly Numeric :any;
                readonly String :any;
                readonly Formula :any;
                readonly Blank :any;
                readonly Boolean :any;
                readonly Error :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    /**
    名前:DateTime
    名前空間:System
    完全限定名:System.DateTime
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, Sealed, Serializable, BeforeFieldInit
    */
    interface DateTime {
        new(ticks: number);
        new(ticks: number, kind: any);
        new(year: number, month: number, day: number);
        new(year: number, month: number, day: number, calendar: any);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number, kind: any);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number, calendar: any);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number, millisecond: number);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number, millisecond: number, kind: any);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number, millisecond: number, calendar: any);
        new(year: number, month: number, day: number, hour: number, minute: number, second: number, millisecond: number, calendar: any, kind: any);
        readonly MinValue :any;
        readonly MaxValue :any;
        readonly Date :any;
        readonly Day :number;
        readonly DayOfWeek :any;
        readonly DayOfYear :number;
        readonly Hour :number;
        readonly Kind :any;
        readonly Millisecond :number;
        readonly Minute :number;
        readonly Month :number;
        readonly Now :any;
        readonly UtcNow :any;
        readonly Second :number;
        readonly Ticks :number;
        readonly TimeOfDay :any;
        readonly Today :any;
        readonly Year :number;
        Add(value: any): any;
        AddDays(value: number): any;
        AddHours(value: number): any;
        AddMilliseconds(value: number): any;
        AddMinutes(value: number): any;
        AddMonths(months: number): any;
        AddSeconds(value: number): any;
        AddTicks(value: number): any;
        AddYears(value: number): any;
        Compare(t1: any, t2: any): number;
        CompareTo(value: any): number;
        DaysInMonth(year: number, month: number): number;
        Equals(value: any): boolean;
        Equals(t1: any, t2: any): boolean;
        FromBinary(dateData: number): any;
        FromFileTime(fileTime: number): any;
        FromFileTimeUtc(fileTime: number): any;
        FromOADate(d: number): any;
        IsDaylightSavingTime(): boolean;
        SpecifyKind(value: any, kind: any): any;
        ToBinary(): number;
        GetHashCode(): number;
        IsLeapYear(year: number): boolean;
        Parse(s: string): any;
        Parse(s: string, provider: any): any;
        Parse(s: string, provider: any, styles: any): any;
        ParseExact(s: string, format: string, provider: any): any;
        ParseExact(s: string, format: string, provider: any, style: any): any;
        ParseExact(s: string, formats: any, provider: any, style: any): any;
        Subtract(value: any): any;
        ToOADate(): number;
        ToFileTime(): number;
        ToFileTimeUtc(): number;
        ToLocalTime(): any;
        ToLongDateString(): string;
        ToLongTimeString(): string;
        ToShortDateString(): string;
        ToShortTimeString(): string;
        ToString(): string;
        ToString(format: string): string;
        ToString(provider: any): string;
        ToString(format: string, provider: any): string;
        ToUniversalTime(): any;
        TryParse(s: string, result: any): boolean;
        TryParse(s: string, provider: any, styles: any, result: any): boolean;
        TryParseExact(s: string, format: string, provider: any, style: any, result: any): boolean;
        TryParseExact(s: string, formats: any, provider: any, style: any, result: any): boolean;
        GetDateTimeFormats(): any;
        GetDateTimeFormats(provider: any): any;
        GetDateTimeFormats(format: any): any;
        GetDateTimeFormats(format: any, provider: any): any;
        GetTypeCode(): any;
        GetType(): any;
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IRichTextString
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IRichTextString
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IRichTextString {
                readonly String :string;
                readonly Length :number;
                readonly NumFormattingRuns :number;
                ApplyFont(startIndex: number, endIndex: number, fontIndex: any): void;
                ApplyFont(startIndex: number, endIndex: number, font: any): void;
                ApplyFont(font: any): void;
                ClearFormatting(): void;
                GetIndexOfFormattingRun(index: number): number;
                ApplyFont(fontIndex: any): void;
            }
        }
    }
}
declare namespace System {
    /**
    名前:Byte
    名前空間:System
    完全限定名:System.Byte
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, SequentialLayout, Sealed, Serializable, BeforeFieldInit
    */
    interface Byte {
        readonly MaxValue :any;
        readonly MinValue :any;
        CompareTo(value: any): number;
        Equals(obj: any): boolean;
        GetHashCode(): number;
        Parse(s: string): any;
        Parse(s: string, style: any): any;
        Parse(s: string, provider: any): any;
        Parse(s: string, style: any, provider: any): any;
        TryParse(s: string, result: any): boolean;
        TryParse(s: string, style: any, provider: any, result: any): boolean;
        ToString(): string;
        ToString(format: string): string;
        ToString(provider: any): string;
        ToString(format: string, provider: any): string;
        GetTypeCode(): any;
        GetType(): any;
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:ICellStyle
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.ICellStyle
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ICellStyle {
                ShrinkToFit :boolean;
                readonly Index :any;
                DataFormat :any;
                readonly FontIndex :any;
                IsHidden :boolean;
                IsLocked :boolean;
                Alignment :any;
                WrapText :boolean;
                VerticalAlignment :any;
                Rotation :any;
                Indention :any;
                BorderLeft :any;
                BorderRight :any;
                BorderTop :any;
                BorderBottom :any;
                LeftBorderColor :any;
                RightBorderColor :any;
                TopBorderColor :any;
                BottomBorderColor :any;
                FillPattern :any;
                FillBackgroundColor :any;
                FillForegroundColor :any;
                BorderDiagonalColor :any;
                BorderDiagonalLineStyle :any;
                BorderDiagonal :any;
                readonly FillBackgroundColorColor :any;
                readonly FillForegroundColorColor :any;
                GetDataFormatString(): string;
                SetFont(font: any): void;
                CloneStyleFrom(source: any): void;
                GetFont(parentWorkbook: any): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IComment
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IComment
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IComment {
                Visible :boolean;
                Row :number;
                Column :number;
                Author :string;
                String :any;
                readonly ClientAnchor :any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IHyperlink
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IHyperlink
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IHyperlink {
                Address :string;
                Label :string;
                readonly Type :any;
                FirstRow :number;
                LastRow :number;
                FirstColumn :number;
                LastColumn :number;
                TextMark :string;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace Util {
            /**
            名前:CellRangeAddress
            名前空間:NPOI.SS.Util
            完全限定名:NPOI.SS.Util.CellRangeAddress
            このメンバを宣言するクラス:
            親クラス:NPOI.SS.Util.CellRangeAddressBase
            属性:AutoLayout, AnsiClass, Class, Public, BeforeFieldInit
            */
            interface CellRangeAddress {
                new(firstRow: number, lastRow: number, firstCol: number, lastCol: number);
                new(in1: any);
                readonly ENCODED_SIZE :number;
                readonly IsFullColumnRange :boolean;
                readonly IsFullRowRange :boolean;
                FirstColumn :number;
                FirstRow :number;
                LastColumn :number;
                LastRow :number;
                readonly NumberOfCells :number;
                Serialize(out1: any): void;
                FormatAsString(): string;
                FormatAsString(sheetName: string, useAbsoluteAddress: boolean): string;
                Copy(): any;
                GetEncodedSize(numberOfItems: number): number;
                ValueOf(reference: string): any;
                Validate(ssVersion: any): void;
                IsInRange(rowInd: number, colInd: number): boolean;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    /**
    名前:Int16
    名前空間:System
    完全限定名:System.Int16
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, SequentialLayout, Sealed, Serializable, BeforeFieldInit
    */
    interface Int16 {
        readonly MaxValue :any;
        readonly MinValue :any;
        CompareTo(value: any): number;
        Equals(obj: any): boolean;
        GetHashCode(): number;
        ToString(): string;
        ToString(provider: any): string;
        ToString(format: string): string;
        ToString(format: string, provider: any): string;
        Parse(s: string): any;
        Parse(s: string, style: any): any;
        Parse(s: string, provider: any): any;
        Parse(s: string, style: any, provider: any): any;
        TryParse(s: string, result: any): boolean;
        TryParse(s: string, style: any, provider: any, result: any): boolean;
        GetTypeCode(): any;
        GetType(): any;
    }
}
declare namespace System {
    /**
    名前:Nullable`1
    名前空間:System
    完全限定名:System.Nullable`1
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, SequentialLayout, Sealed, Serializable, BeforeFieldInit
    */
    interface Nullable<T> {
        new(value: any);
        readonly HasValue :boolean;
        readonly Value :any;
        GetValueOrDefault(): any;
        GetValueOrDefault(defaultValue: any): any;
        Equals(other: any): boolean;
        GetHashCode(): number;
        ToString(): string;
        GetType(): any;
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:MissingCellPolicy
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.MissingCellPolicy
            このメンバを宣言するクラス:
            親クラス:System.Object
            属性:AutoLayout, AnsiClass, Class, Public, BeforeFieldInit
            */
            interface MissingCellPolicy {
                new();
                new(policy: any);
                id :number;
                _policy :any;
                readonly RETURN_NULL_AND_BLANK :any;
                readonly RETURN_BLANK_AS_NULL :any;
                readonly CREATE_NULL_AS_BLANK :any;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IPrintSetup
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IPrintSetup
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IPrintSetup {
                PaperSize :any;
                Scale :any;
                PageStart :any;
                FitWidth :any;
                FitHeight :any;
                LeftToRight :boolean;
                Landscape :boolean;
                ValidSettings :boolean;
                NoColor :boolean;
                Draft :boolean;
                Notes :boolean;
                NoOrientation :boolean;
                UsePage :boolean;
                HResolution :any;
                VResolution :any;
                HeaderMargin :number;
                FooterMargin :number;
                Copies :any;
                EndNote :boolean;
                CellError :any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IHeader
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IHeader
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IHeader {
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IFooter
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IFooter
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IFooter {
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IDrawing
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IDrawing
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IDrawing {
                CreatePicture(anchor: any, pictureIndex: number): any;
                CreateCellComment(anchor: any): any;
                CreateChart(anchor: any): any;
                CreateAnchor(dx1: number, dy1: number, dx2: number, dy2: number, col1: number, row1: number, col2: number, row2: number): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace Util {
            /**
            名前:PaneInformation
            名前空間:NPOI.SS.Util
            完全限定名:NPOI.SS.Util.PaneInformation
            このメンバを宣言するクラス:
            親クラス:System.Object
            属性:AutoLayout, AnsiClass, Class, Public, BeforeFieldInit
            */
            interface PaneInformation {
                new(x: any, y: any, top: any, left: any, active: any, frozen: boolean);
                readonly PANE_LOWER_RIGHT :any;
                readonly PANE_UPPER_RIGHT :any;
                readonly PANE_LOWER_LEFT :any;
                readonly PANE_UPPER_LEFT :any;
                readonly VerticalSplitPosition :any;
                readonly HorizontalSplitPosition :any;
                readonly HorizontalSplitTopRow :any;
                readonly VerticalSplitLeftColumn :any;
                readonly ActivePane :any;
                IsFreezePane(): boolean;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:ISheetConditionalFormatting
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.ISheetConditionalFormatting
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ISheetConditionalFormatting {
                readonly NumConditionalFormattings :number;
                AddConditionalFormatting(regions: any, rule: any): number;
                AddConditionalFormatting(regions: any, rule1: any, rule2: any): number;
                AddConditionalFormatting(regions: any, cfRules: any): number;
                AddConditionalFormatting(cf: any): number;
                CreateConditionalFormattingRule(comparisonOperation: any, formula1: string, formula2: string): any;
                CreateConditionalFormattingRule(comparisonOperation: any, formula: string): any;
                CreateConditionalFormattingRule(formula: string): any;
                GetConditionalFormattingAt(index: number): any;
                RemoveConditionalFormatting(index: number): void;
            }
        }
    }
}
declare namespace System {
    namespace Collections {
        /**
        名前:IEnumerator
        名前空間:System.Collections
        完全限定名:System.Collections.IEnumerator
        このメンバを宣言するクラス:
        親クラス:
        属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
        */
        interface IEnumerator {
            readonly Current :any;
            MoveNext(): boolean;
            Reset(): void;
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:MarginType
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.MarginType
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface MarginType {
                value__ :any;
                readonly LeftMargin :any;
                readonly RightMargin :any;
                readonly TopMargin :any;
                readonly BottomMargin :any;
                readonly HeaderMargin :any;
                readonly FooterMargin :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:PanePosition
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.PanePosition
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface PanePosition {
                value__ :any;
                readonly LowerRight :any;
                readonly UpperRight :any;
                readonly LowerLeft :any;
                readonly UpperLeft :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace Util {
            /**
            名前:CellRangeAddress8Bit
            名前空間:NPOI.SS.Util
            完全限定名:NPOI.SS.Util.CellRangeAddress8Bit
            このメンバを宣言するクラス:
            親クラス:NPOI.SS.Util.CellRangeAddressBase
            属性:AutoLayout, AnsiClass, Class, Public, BeforeFieldInit
            */
            interface CellRangeAddress8Bit {
                new(firstRow: number, lastRow: number, firstCol: number, lastCol: number);
                new(in1: any);
                readonly ENCODED_SIZE :number;
                readonly IsFullColumnRange :boolean;
                readonly IsFullRowRange :boolean;
                FirstColumn :number;
                FirstRow :number;
                LastColumn :number;
                LastRow :number;
                readonly NumberOfCells :number;
                Serialize(offset: number, data: any): number;
                Serialize(out1: any): void;
                Copy(): any;
                GetEncodedSize(numberOfItems: number): number;
                Validate(ssVersion: any): void;
                IsInRange(rowInd: number, colInd: number): boolean;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:ICellRange`1
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.ICellRange`1
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ICellRange<T> {
                readonly Width :number;
                readonly Height :number;
                readonly Size :number;
                readonly ReferenceText :string;
                readonly TopLeftCell :any;
                readonly FlattenedCells :any;
                readonly Cells :any;
                GetCell(relativeRowIndex: number, relativeColumnIndex: number): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IDataValidationHelper
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IDataValidationHelper
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IDataValidationHelper {
                CreateFormulaListConstraint(listFormula: string): any;
                CreateExplicitListConstraint(listOfValues: any): any;
                CreateNumericConstraint(validationType: number, operatorType: number, formula1: string, formula2: string): any;
                CreateTextLengthConstraint(operatorType: number, formula1: string, formula2: string): any;
                CreateDecimalConstraint(operatorType: number, formula1: string, formula2: string): any;
                CreateintConstraint(operatorType: number, formula1: string, formula2: string): any;
                CreateDateConstraint(operatorType: number, formula1: string, formula2: string, dateFormat: string): any;
                CreateTimeConstraint(operatorType: number, formula1: string, formula2: string): any;
                CreateCustomConstraint(formula: string): any;
                CreateValidation(constraint: any, cellRangeAddressList: any): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IDataValidation
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IDataValidation
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IDataValidation {
                readonly ValidationConstraint :any;
                ErrorStyle :number;
                EmptyCellAllowed :boolean;
                SuppressDropDownArrow :boolean;
                ShowPromptBox :boolean;
                ShowErrorBox :boolean;
                readonly PromptBoxTitle :string;
                readonly PromptBoxText :string;
                readonly ErrorBoxTitle :string;
                readonly ErrorBoxText :string;
                readonly Regions :any;
                CreatePromptBox(title: string, text: string): void;
                CreateErrorBox(title: string, text: string): void;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IAutoFilter
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IAutoFilter
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IAutoFilter {
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IFont
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IFont
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IFont {
                FontName :string;
                FontHeight :number;
                FontHeightInPoints :any;
                IsItalic :boolean;
                IsStrikeout :boolean;
                Color :any;
                TypeOffset :any;
                Underline :any;
                Charset :any;
                readonly Index :any;
                Boldweight :any;
                IsBold :boolean;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:FontSuperScript
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.FontSuperScript
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface FontSuperScript {
                value__ :any;
                readonly None :any;
                readonly Super :any;
                readonly Sub :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:FontUnderlineType
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.FontUnderlineType
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface FontUnderlineType {
                value__ :any;
                readonly None :any;
                readonly Single :any;
                readonly Double :any;
                readonly SingleAccounting :any;
                readonly DoubleAccounting :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    namespace IO {
        /**
        名前:Stream
        名前空間:System.IO
        完全限定名:System.IO.Stream
        このメンバを宣言するクラス:
        親クラス:System.MarshalByRefObject
        属性:AutoLayout, AnsiClass, Class, Public, Abstract, Serializable, BeforeFieldInit
        */
        interface Stream {
            readonly Null :any;
            readonly CanRead :boolean;
            readonly CanSeek :boolean;
            readonly CanTimeout :boolean;
            readonly CanWrite :boolean;
            readonly Length :number;
            Position :number;
            ReadTimeout :number;
            WriteTimeout :number;
            CopyToAsync(destination: any): any;
            CopyToAsync(destination: any, bufferSize: number): any;
            CopyToAsync(destination: any, bufferSize: number, cancellationToken: any): any;
            CopyTo(destination: any): void;
            CopyTo(destination: any, bufferSize: number): void;
            Close(): void;
            Dispose(): void;
            Flush(): void;
            FlushAsync(): any;
            FlushAsync(cancellationToken: any): any;
            BeginRead(buffer: any, offset: number, count: number, callback: any, state: any): any;
            EndRead(asyncResult: any): number;
            ReadAsync(buffer: any, offset: number, count: number): any;
            ReadAsync(buffer: any, offset: number, count: number, cancellationToken: any): any;
            BeginWrite(buffer: any, offset: number, count: number, callback: any, state: any): any;
            EndWrite(asyncResult: any): void;
            WriteAsync(buffer: any, offset: number, count: number): any;
            WriteAsync(buffer: any, offset: number, count: number, cancellationToken: any): any;
            Seek(offset: number, origin: any): number;
            SetLength(value: number): void;
            Read(buffer: any, offset: number, count: number): number;
            ReadByte(): number;
            Write(buffer: any, offset: number, count: number): void;
            WriteByte(value: any): void;
            Synchronized(stream: any): any;
            GetLifetimeService(): any;
            InitializeLifetimeService(): any;
            CreateObjRef(requestedType: any): any;
            ToString(): string;
            Equals(obj: any): boolean;
            GetHashCode(): number;
            GetType(): any;
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IName
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IName
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IName {
                readonly SheetName :string;
                NameName :string;
                RefersToFormula :string;
                readonly IsFunctionName :boolean;
                readonly IsDeleted :boolean;
                SheetIndex :number;
                Comment :string;
                SetFunction(value: boolean): void;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:IDataFormat
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.IDataFormat
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IDataFormat {
                GetFormat(format: string): any;
                GetFormat(index: any): string;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:PictureType
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.PictureType
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface PictureType {
                value__ :number;
                readonly Unknown :any;
                readonly None :any;
                readonly EMF :any;
                readonly WMF :any;
                readonly PICT :any;
                readonly JPEG :any;
                readonly PNG :any;
                readonly DIB :any;
                readonly GIF :any;
                readonly TIFF :any;
                readonly EPS :any;
                readonly BMP :any;
                readonly WPG :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    namespace Collections {
        /**
        名前:IList
        名前空間:System.Collections
        完全限定名:System.Collections.IList
        このメンバを宣言するクラス:
        親クラス:
        属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
        */
        interface IList {
            Item :any;
            readonly IsReadOnly :boolean;
            readonly IsFixedSize :boolean;
            [index: number]: any;
            Add(value: any): number;
            Contains(value: any): boolean;
            Clear(): void;
            IndexOf(value: any): number;
            Insert(index: number, value: any): void;
            Remove(value: any): void;
            RemoveAt(index: number): void;
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:ICreationHelper
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.ICreationHelper
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface ICreationHelper {
                CreateRichTextString(text: string): any;
                CreateDataFormat(): any;
                CreateHyperlink(type: any): any;
                CreateFormulaEvaluator(): any;
                CreateClientAnchor(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace UserModel {
            /**
            名前:SheetState
            名前空間:NPOI.SS.UserModel
            完全限定名:NPOI.SS.UserModel.SheetState
            このメンバを宣言するクラス:
            親クラス:System.Enum
            属性:AutoLayout, AnsiClass, Class, Public, Sealed
            */
            interface SheetState {
                value__ :number;
                readonly Visible :any;
                readonly Hidden :any;
                readonly VeryHidden :any;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                ToString(): string;
                ToString(format: string, provider: any): string;
                CompareTo(target: any): number;
                ToString(format: string): string;
                ToString(provider: any): string;
                HasFlag(flag: any): boolean;
                GetTypeCode(): any;
                GetType(): any;
            }
        }
    }
}
declare namespace NPOI {
    namespace SS {
        namespace Formula {
            namespace Udf {
                /**
                名前:UDFFinder
                名前空間:NPOI.SS.Formula.Udf
                完全限定名:NPOI.SS.Formula.Udf.UDFFinder
                このメンバを宣言するクラス:
                親クラス:System.Object
                属性:AutoLayout, AnsiClass, Class, Public, Abstract, BeforeFieldInit
                */
                interface UDFFinder {
                    readonly DEFAULT :any;
                    FindFunction(name: string): any;
                    ToString(): string;
                    Equals(obj: any): boolean;
                    GetHashCode(): number;
                    GetType(): any;
                }
            }
        }
    }
}
declare namespace System {
    /**
    名前:Char
    名前空間:System
    完全限定名:System.Char
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, SequentialLayout, Sealed, Serializable, BeforeFieldInit
    */
    interface Char {
        readonly MaxValue :any;
        readonly MinValue :any;
        GetHashCode(): number;
        Equals(obj: any): boolean;
        CompareTo(value: any): number;
        ToString(): string;
        ToString(provider: any): string;
        ToString(c: any): string;
        Parse(s: string): any;
        TryParse(s: string, result: any): boolean;
        IsDigit(c: any): boolean;
        IsLetter(c: any): boolean;
        IsWhiteSpace(c: any): boolean;
        IsUpper(c: any): boolean;
        IsLower(c: any): boolean;
        IsPunctuation(c: any): boolean;
        IsLetterOrDigit(c: any): boolean;
        ToUpper(c: any, culture: any): any;
        ToUpper(c: any): any;
        ToUpperInvariant(c: any): any;
        ToLower(c: any, culture: any): any;
        ToLower(c: any): any;
        ToLowerInvariant(c: any): any;
        GetTypeCode(): any;
        IsControl(c: any): boolean;
        IsControl(s: string, index: number): boolean;
        IsDigit(s: string, index: number): boolean;
        IsLetter(s: string, index: number): boolean;
        IsLetterOrDigit(s: string, index: number): boolean;
        IsLower(s: string, index: number): boolean;
        IsNumber(c: any): boolean;
        IsNumber(s: string, index: number): boolean;
        IsPunctuation(s: string, index: number): boolean;
        IsSeparator(c: any): boolean;
        IsSeparator(s: string, index: number): boolean;
        IsSurrogate(c: any): boolean;
        IsSurrogate(s: string, index: number): boolean;
        IsSymbol(c: any): boolean;
        IsSymbol(s: string, index: number): boolean;
        IsUpper(s: string, index: number): boolean;
        IsWhiteSpace(s: string, index: number): boolean;
        GetUnicodeCategory(c: any): any;
        GetUnicodeCategory(s: string, index: number): any;
        GetNumericValue(c: any): number;
        GetNumericValue(s: string, index: number): number;
        IsHighSurrogate(c: any): boolean;
        IsHighSurrogate(s: string, index: number): boolean;
        IsLowSurrogate(c: any): boolean;
        IsLowSurrogate(s: string, index: number): boolean;
        IsSurrogatePair(s: string, index: number): boolean;
        IsSurrogatePair(highSurrogate: any, lowSurrogate: any): boolean;
        ConvertFromUtf32(utf32: number): string;
        ConvertToUtf32(highSurrogate: any, lowSurrogate: any): number;
        ConvertToUtf32(s: string, index: number): number;
        GetType(): any;
    }
}
declare namespace System {
    namespace Text {
        /**
        名前:Encoding
        名前空間:System.Text
        完全限定名:System.Text.Encoding
        このメンバを宣言するクラス:
        親クラス:System.Object
        属性:AutoLayout, AnsiClass, Class, Public, Abstract, Serializable, BeforeFieldInit
        */
        interface Encoding {
            readonly BodyName :string;
            readonly EncodingName :string;
            readonly HeaderName :string;
            readonly WebName :string;
            readonly WindowsCodePage :number;
            readonly IsBrowserDisplay :boolean;
            readonly IsBrowserSave :boolean;
            readonly IsMailNewsDisplay :boolean;
            readonly IsMailNewsSave :boolean;
            readonly IsSingleByte :boolean;
            EncoderFallback :any;
            DecoderFallback :any;
            readonly IsReadOnly :boolean;
            readonly ASCII :any;
            readonly CodePage :number;
            readonly Default :any;
            readonly Unicode :any;
            readonly BigEndianUnicode :any;
            readonly UTF7 :any;
            readonly UTF8 :any;
            readonly UTF32 :any;
            Convert(srcEncoding: any, dstEncoding: any, bytes: any): any;
            Convert(srcEncoding: any, dstEncoding: any, bytes: any, index: number, count: number): any;
            RegisterProvider(provider: any): void;
            GetEncoding(codepage: number): any;
            GetEncoding(codepage: number, encoderFallback: any, decoderFallback: any): any;
            GetEncoding(name: string): any;
            GetEncoding(name: string, encoderFallback: any, decoderFallback: any): any;
            GetEncodings(): any;
            GetPreamble(): any;
            Clone(): any;
            GetByteCount(chars: any): number;
            GetByteCount(s: string): number;
            GetByteCount(chars: any, index: number, count: number): number;
            GetByteCount(chars: any, count: number): number;
            GetBytes(chars: any): any;
            GetBytes(chars: any, index: number, count: number): any;
            GetBytes(chars: any, charIndex: number, charCount: number, bytes: any, byteIndex: number): number;
            GetBytes(s: string): any;
            GetBytes(s: string, charIndex: number, charCount: number, bytes: any, byteIndex: number): number;
            GetBytes(chars: any, charCount: number, bytes: any, byteCount: number): number;
            GetCharCount(bytes: any): number;
            GetCharCount(bytes: any, index: number, count: number): number;
            GetCharCount(bytes: any, count: number): number;
            GetChars(bytes: any): any;
            GetChars(bytes: any, index: number, count: number): any;
            GetChars(bytes: any, byteIndex: number, byteCount: number, chars: any, charIndex: number): number;
            GetChars(bytes: any, byteCount: number, chars: any, charCount: number): number;
            GetString(bytes: any, byteCount: number): string;
            IsAlwaysNormalized(): boolean;
            IsAlwaysNormalized(form: any): boolean;
            GetDecoder(): any;
            GetEncoder(): any;
            GetMaxByteCount(charCount: number): number;
            GetMaxCharCount(byteCount: number): number;
            GetString(bytes: any): string;
            GetString(bytes: any, index: number, count: number): string;
            Equals(value: any): boolean;
            GetHashCode(): number;
            ToString(): string;
            GetType(): any;
        }
    }
}
declare namespace System {
    /**
    名前:IFormatProvider
    名前空間:System
    完全限定名:System.IFormatProvider
    このメンバを宣言するクラス:
    親クラス:
    属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
    */
    interface IFormatProvider {
        GetFormat(formatType: any): any;
    }
}
declare namespace System {
    namespace Threading {
        namespace Tasks {
            /**
            名前:Task
            名前空間:System.Threading.Tasks
            完全限定名:System.Threading.Tasks.Task
            このメンバを宣言するクラス:
            親クラス:System.Object
            属性:AutoLayout, AnsiClass, Class, Public, HasSecurity, BeforeFieldInit
            */
            interface Task {
                new(action: any);
                new(action: any, cancellationToken: any);
                new(action: any, creationOptions: any);
                new(action: any, cancellationToken: any, creationOptions: any);
                new<D>(action: any, state: any);
                new<D>(action: any, state: any, cancellationToken: any);
                new<D>(action: any, state: any, creationOptions: any);
                new<D>(action: any, state: any, cancellationToken: any, creationOptions: any);
                readonly Id :number;
                readonly CurrentId :any;
                readonly Exception :any;
                readonly Status :any;
                readonly IsCanceled :boolean;
                readonly IsCompleted :boolean;
                readonly CreationOptions :any;
                readonly AsyncState :any;
                readonly Factory :any;
                readonly CompletedTask :any;
                readonly IsFaulted :boolean;
                Start(): void;
                Start(scheduler: any): void;
                RunSynchronously(): void;
                RunSynchronously(scheduler: any): void;
                Dispose(): void;
                GetAwaiter(): any;
                ConfigureAwait(continueOnCapturedContext: boolean): any;
                Yield(): any;
                Wait(): void;
                Wait(timeout: any): boolean;
                Wait(cancellationToken: any): void;
                Wait(millisecondsTimeout: number): boolean;
                Wait(millisecondsTimeout: number, cancellationToken: any): boolean;
                ContinueWith<D>(continuationAction: any): any;
                ContinueWith<D>(continuationAction: any, cancellationToken: any): any;
                ContinueWith<D>(continuationAction: any, scheduler: any): any;
                ContinueWith<D>(continuationAction: any, continuationOptions: any): any;
                ContinueWith<D>(continuationAction: any, cancellationToken: any, continuationOptions: any, scheduler: any): any;
                ContinueWith<D>(continuationAction: any, state: any): any;
                ContinueWith<D>(continuationAction: any, state: any, cancellationToken: any): any;
                ContinueWith<D>(continuationAction: any, state: any, scheduler: any): any;
                ContinueWith<D>(continuationAction: any, state: any, continuationOptions: any): any;
                ContinueWith<D>(continuationAction: any, state: any, cancellationToken: any, continuationOptions: any, scheduler: any): any;
                ContinueWith<D, TResult>(continuationFunction: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, cancellationToken: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, scheduler: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, continuationOptions: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, cancellationToken: any, continuationOptions: any, scheduler: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, state: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, state: any, cancellationToken: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, state: any, scheduler: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, state: any, continuationOptions: any): any;
                ContinueWith<D, TResult>(continuationFunction: any, state: any, cancellationToken: any, continuationOptions: any, scheduler: any): any;
                WaitAll(...tasks: any[]): void;
                WaitAll(tasks: any, timeout: any): boolean;
                WaitAll(tasks: any, millisecondsTimeout: number): boolean;
                WaitAll(tasks: any, cancellationToken: any): void;
                WaitAll(tasks: any, millisecondsTimeout: number, cancellationToken: any): boolean;
                WaitAny(...tasks: any[]): number;
                WaitAny(tasks: any, timeout: any): number;
                WaitAny(tasks: any, cancellationToken: any): number;
                WaitAny(tasks: any, millisecondsTimeout: number): number;
                WaitAny(tasks: any, millisecondsTimeout: number, cancellationToken: any): number;
                FromResult<TResult>(result: any): any;
                FromException(exception: any): any;
                FromException<TResult>(exception: any): any;
                Run(action: any): any;
                Run(action: any, cancellationToken: any): any;
                Run<TResult>(_function: any): any;
                Run<TResult>(_function: any, cancellationToken: any): any;
                Run<D>(_function: any): any;
                Run<D>(_function: any, cancellationToken: any): any;
                Run<D, TResult>(_function: any): any;
                Run<D, TResult>(_function: any, cancellationToken: any): any;
                Delay(delay: any): any;
                Delay(delay: any, cancellationToken: any): any;
                Delay(millisecondsDelay: number): any;
                Delay(millisecondsDelay: number, cancellationToken: any): any;
                WhenAll<D>(tasks: any): any;
                WhenAll(...tasks: any[]): any;
                WhenAll<D, TResult>(tasks: any): any;
                WhenAll<TResult>(...tasks: any[]): any;
                WhenAny(...tasks: any[]): any;
                WhenAny<D>(tasks: any): any;
                WhenAny<TResult>(...tasks: any[]): any;
                WhenAny<D, TResult>(tasks: any): any;
                FromCanceled(cancellationToken: any): any;
                FromCanceled<TResult>(cancellationToken: any): any;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    namespace Runtime {
        namespace Remoting {
            /**
            名前:ObjRef
            名前空間:System.Runtime.Remoting
            完全限定名:System.Runtime.Remoting.ObjRef
            このメンバを宣言するクラス:
            親クラス:System.Object
            属性:AutoLayout, AnsiClass, Class, Public, Serializable, HasSecurity, BeforeFieldInit
            */
            interface ObjRef {
                new(o: any, requestedType: any);
                new();
                URI :string;
                TypeInfo :any;
                EnvoyInfo :any;
                ChannelInfo :any;
                GetRealObject(context: any): any;
                GetObjectData(info: any, context: any): void;
                IsFromThisProcess(): boolean;
                IsFromThisAppDomain(): boolean;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    /**
    名前:SByte
    名前空間:System
    完全限定名:System.SByte
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, SequentialLayout, Sealed, Serializable, BeforeFieldInit
    */
    interface SByte {
        readonly MaxValue :any;
        readonly MinValue :any;
        ToString(): string;
        CompareTo(obj: any): number;
        CompareTo(value: any): number;
        Equals(obj: any): boolean;
        GetHashCode(): number;
        ToString(provider: any): string;
        ToString(format: string): string;
        ToString(format: string, provider: any): string;
        Parse(s: string): any;
        Parse(s: string, style: any): any;
        Parse(s: string, provider: any): any;
        Parse(s: string, style: any, provider: any): any;
        TryParse(s: string, result: any): boolean;
        TryParse(s: string, style: any, provider: any, result: any): boolean;
        GetTypeCode(): any;
        GetType(): any;
    }
}
declare namespace System {
    /**
    名前:UInt16
    名前空間:System
    完全限定名:System.UInt16
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, SequentialLayout, Sealed, Serializable, BeforeFieldInit
    */
    interface UInt16 {
        readonly MaxValue :any;
        readonly MinValue :any;
        CompareTo(value: any): number;
        Equals(obj: any): boolean;
        GetHashCode(): number;
        ToString(): string;
        ToString(provider: any): string;
        ToString(format: string): string;
        ToString(format: string, provider: any): string;
        Parse(s: string): any;
        Parse(s: string, style: any): any;
        Parse(s: string, provider: any): any;
        Parse(s: string, style: any, provider: any): any;
        TryParse(s: string, result: any): boolean;
        TryParse(s: string, style: any, provider: any, result: any): boolean;
        GetTypeCode(): any;
        GetType(): any;
    }
}
declare namespace System {
    /**
    名前:Enum
    名前空間:System
    完全限定名:System.Enum
    このメンバを宣言するクラス:
    親クラス:System.ValueType
    属性:AutoLayout, AnsiClass, Class, Public, Abstract, Serializable, BeforeFieldInit
    */
    interface Enum {
        TryParse<TEnum>(value: string, result: any): boolean;
        TryParse<TEnum>(value: string, ignoreCase: boolean, result: any): boolean;
        Parse(enumType: any, value: string): any;
        Parse(enumType: any, value: string, ignoreCase: boolean): any;
        GetUnderlyingType(enumType: any): any;
        GetValues(enumType: any): any;
        GetName(enumType: any, value: any): string;
        GetNames(enumType: any): any;
        ToObject(enumType: any, value: any): any;
        IsDefined(enumType: any, value: any): boolean;
        Format(enumType: any, value: any, format: string): string;
        Equals(obj: any): boolean;
        GetHashCode(): number;
        ToString(): string;
        ToString(format: string, provider: any): string;
        CompareTo(target: any): number;
        ToString(format: string): string;
        ToString(provider: any): string;
        HasFlag(flag: any): boolean;
        GetTypeCode(): any;
        ToObject(enumType: any, value: number): any;
        GetType(): any;
    }
}
declare namespace System {
    /**
    名前:TypeCode
    名前空間:System
    完全限定名:System.TypeCode
    このメンバを宣言するクラス:
    親クラス:System.Enum
    属性:AutoLayout, AnsiClass, Class, Public, Sealed, Serializable
    */
    interface TypeCode {
        value__ :number;
        readonly Empty :any;
        readonly Object :any;
        readonly DBNull :any;
        readonly Boolean :any;
        readonly Char :any;
        readonly SByte :any;
        readonly Byte :any;
        readonly Int16 :any;
        readonly UInt16 :any;
        readonly Int32 :any;
        readonly UInt32 :any;
        readonly Int64 :any;
        readonly UInt64 :any;
        readonly Single :any;
        readonly Double :any;
        readonly Decimal :any;
        readonly DateTime :any;
        readonly String :any;
        Equals(obj: any): boolean;
        GetHashCode(): number;
        ToString(): string;
        ToString(format: string, provider: any): string;
        CompareTo(target: any): number;
        ToString(format: string): string;
        ToString(provider: any): string;
        HasFlag(flag: any): boolean;
        GetTypeCode(): any;
        GetType(): any;
    }
}
declare namespace System {
    namespace Windows {
        namespace Forms {
            /**
            名前:FileDialogCustomPlacesCollection
            名前空間:System.Windows.Forms
            完全限定名:System.Windows.Forms.FileDialogCustomPlacesCollection
            このメンバを宣言するクラス:
            親クラス:System.Collections.ObjectModel.Collection`1[System.Windows.Forms.FileDialogCustomPlace]
            属性:AutoLayout, AnsiClass, Class, Public, BeforeFieldInit
            */
            interface FileDialogCustomPlacesCollection {
                new();
                readonly Count :number;
                Item :any;
                Add(path: string): void;
                Add(knownFolderGuid: any): void;
                [index: number]: any;
                Add(item: any): void;
                Clear(): void;
                CopyTo(array: any, index: number): void;
                Contains(item: any): boolean;
                GetEnumerator(): any;
                [Symbol.iterator](): IterableIterator<any>;
                IndexOf(item: any): number;
                Insert(index: number, item: any): void;
                Remove(item: any): boolean;
                RemoveAt(index: number): void;
                ToString(): string;
                Equals(obj: any): boolean;
                GetHashCode(): number;
                GetType(): any;
            }
        }
    }
}
declare namespace System {
    namespace ComponentModel {
        /**
        名前:ISite
        名前空間:System.ComponentModel
        完全限定名:System.ComponentModel.ISite
        このメンバを宣言するクラス:
        親クラス:
        属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
        */
        interface ISite {
            readonly Component :any;
            readonly Container :any;
            readonly DesignMode :boolean;
            Name :string;
        }
    }
}
declare namespace System {
    namespace ComponentModel {
        /**
        名前:IContainer
        名前空間:System.ComponentModel
        完全限定名:System.ComponentModel.IContainer
        このメンバを宣言するクラス:
        親クラス:
        属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
        */
        interface IContainer {
            readonly Components :any;
            Add(component: any): void;
            Add(component: any, name: string): void;
            Remove(component: any): void;
        }
    }
}
declare namespace System {
    namespace ComponentModel {
        /**
        名前:CancelEventHandler
        名前空間:System.ComponentModel
        完全限定名:System.ComponentModel.CancelEventHandler
        このメンバを宣言するクラス:
        親クラス:System.MulticastDelegate
        属性:AutoLayout, AnsiClass, Class, Public, Sealed, HasSecurity
        */
        interface CancelEventHandler {
            new(object: any, method: any);
            readonly Method :any;
            readonly Target :any;
            Invoke(sender: any, e: any): void;
            connect(func: (sender: any, e: any) => void) :void
            disconnect(func: (sender: any, e: any) => void) :void
            BeginInvoke(sender: any, e: any, callback: any, object: any): any;
            EndInvoke(result: any): void;
            GetObjectData(info: any, context: any): void;
            Equals(obj: any): boolean;
            GetInvocationList(): any;
            GetHashCode(): number;
            DynamicInvoke(...args: any[]): any;
            Clone(): any;
            ToString(): string;
            GetType(): any;
        }
    }
}
declare namespace System {
    /**
    名前:EventHandler
    名前空間:System
    完全限定名:System.EventHandler
    このメンバを宣言するクラス:
    親クラス:System.MulticastDelegate
    属性:AutoLayout, AnsiClass, Class, Public, Sealed, Serializable
    */
    interface EventHandler {
        new(object: any, method: any);
        readonly Method :any;
        readonly Target :any;
        Invoke(sender: any, e: any): void;
        connect(func: (sender: any, e: any) => void) :void
        disconnect(func: (sender: any, e: any) => void) :void
        BeginInvoke(sender: any, e: any, callback: any, object: any): any;
        EndInvoke(result: any): void;
        GetObjectData(info: any, context: any): void;
        Equals(obj: any): boolean;
        GetInvocationList(): any;
        GetHashCode(): number;
        DynamicInvoke(...args: any[]): any;
        Clone(): any;
        ToString(): string;
        GetType(): any;
    }
}
declare namespace System {
    namespace Windows {
        namespace Forms {
            /**
            名前:IWin32Window
            名前空間:System.Windows.Forms
            完全限定名:System.Windows.Forms.IWin32Window
            このメンバを宣言するクラス:
            親クラス:
            属性:AutoLayout, AnsiClass, Class, Public, ClassSemanticsMask, Abstract
            */
            interface IWin32Window {
                readonly Handle :any;
            }
        }
    }
}
