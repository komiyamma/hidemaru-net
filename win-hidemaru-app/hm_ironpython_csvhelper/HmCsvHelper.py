# coding: cp932

#
# This codes is licensed under CC0 1.0 Universal
# このcsvhelper.pyはただのサンプルコードです。ご自由にお使いください。
#

# 途中エラーでもクリアしておかねばならない
CsvAllData = []
RowCount = 0


import clr
import sys
sys.path.append(hm.Macro.Var["currentmacrodirectory"])
clr.AddReferenceByPartialName( "CsvHelper")
clr.AddReferenceByPartialName( "System.Windows.Forms" )

import System.IO
import System.Text
import System.Collections.Generic

from CsvHelper import *

# デバッグ用途
__PYDEBUG__ = 0
if __PYDEBUG__:
	class HM:
	    def debuginfo(self, str):
	        print(str+"\n")

	hm = HM()
	target_csv_filename = r"C:\usr\hidemaru\csvhelper\a.csv"


try:
    target_csv_filename = target_csv_filename
except:
    System.Windows.Forms.MessageBox.Show("ファイル名が指定されていません。", "エラー", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error)
    raise IOError


try:
    target_csv_encoding = target_csv_encoding
except:
    target_csv_encoding = "shift_jis"
    hm.debuginfo("エンコーディングの指定が無いため、csvはCP932(SJIS)の文字コードとみなします")


try:
    encoding = System.Text.Encoding.GetEncoding(target_csv_encoding)
except:
    hm.debuginfo("存在しないエンコーディング名の指定。CP932(SJIS)の文字コードとみなします")
    target_csv_encoding = "shift_jis"
    encoding = System.Text.Encoding.GetEncoding(target_csv_encoding)


try:
    print(target_csv_filename)
    file_stream = System.IO.StreamReader(target_csv_filename, encoding)
    parser = CsvParser(file_stream);
except Exception:
    System.Windows.Forms.MessageBox.Show("CSVファイルの読み取りエラー", "エラー", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error)
    System.Console.WriteLine(Exception)
    if file_stream:
        file_stream.Close()
    raise IOError



RowCount = 0
while (True):
    try:
        row_data = parser.Read();
        if row_data == None:
            break;

        # 全体のデータに１行足す
        CsvAllData.append(row_data[:])
        print(row_data)
        RowCount = RowCount + 1
    except:
        break;


file_stream.Close();

# データ全体の行数
def GetRowCount():
    return RowCount

# 対象の１行内のデータ数を得る
def GetColCount(row_ix):
    try:
        global CsvAllData
        row_data = CsvAllData[row_ix]
        return row_data.Count
    except:
        hm.debuginfo("データの範囲外です")
        raise IndexError


# 指定のセルデータを得る
def GetCellItem(row_ix, col_ix):
    try:
        return CsvAllData[row_ix][col_ix]
    except:
        hm.debuginfo("データの範囲外です")
        return ""

