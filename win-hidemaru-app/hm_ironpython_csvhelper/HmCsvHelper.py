# coding: cp932

#
# This codes is licensed under CC0 1.0 Universal
# ����csvhelper.py�͂����̃T���v���R�[�h�ł��B�����R�ɂ��g�����������B
#

# �r���G���[�ł��N���A���Ă����˂΂Ȃ�Ȃ�
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

# �f�o�b�O�p�r
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
    System.Windows.Forms.MessageBox.Show("�t�@�C�������w�肳��Ă��܂���B", "�G���[", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error)
    raise IOError


try:
    target_csv_encoding = target_csv_encoding
except:
    target_csv_encoding = "shift_jis"
    hm.debuginfo("�G���R�[�f�B���O�̎w�肪�������߁Acsv��CP932(SJIS)�̕����R�[�h�Ƃ݂Ȃ��܂�")


try:
    encoding = System.Text.Encoding.GetEncoding(target_csv_encoding)
except:
    hm.debuginfo("���݂��Ȃ��G���R�[�f�B���O���̎w��BCP932(SJIS)�̕����R�[�h�Ƃ݂Ȃ��܂�")
    target_csv_encoding = "shift_jis"
    encoding = System.Text.Encoding.GetEncoding(target_csv_encoding)


try:
    print(target_csv_filename)
    file_stream = System.IO.StreamReader(target_csv_filename, encoding)
    parser = CsvParser(file_stream);
except Exception:
    System.Windows.Forms.MessageBox.Show("CSV�t�@�C���̓ǂݎ��G���[", "�G���[", System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error)
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

        # �S�̂̃f�[�^�ɂP�s����
        CsvAllData.append(row_data[:])
        print(row_data)
        RowCount = RowCount + 1
    except:
        break;


file_stream.Close();

# �f�[�^�S�̂̍s��
def GetRowCount():
    return RowCount

# �Ώۂ̂P�s���̃f�[�^���𓾂�
def GetColCount(row_ix):
    try:
        global CsvAllData
        row_data = CsvAllData[row_ix]
        return row_data.Count
    except:
        hm.debuginfo("�f�[�^�͈̔͊O�ł�")
        raise IndexError


# �w��̃Z���f�[�^�𓾂�
def GetCellItem(row_ix, col_ix):
    try:
        return CsvAllData[row_ix][col_ix]
    except:
        hm.debuginfo("�f�[�^�͈̔͊O�ł�")
        return ""

