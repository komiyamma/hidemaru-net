# coding: utf-8

#--------------------------------------------------------------
# hmPy 1.7.4.1用 フェイクライブラリ
# Copyright (c) 2016-2020 Akitsugu Komiyama
# under the Apache License Version 2.0
#--------------------------------------------------------------
import os

class _TText:
    class _TEncoding:
        def __init__(self, pyencodingname, codepage, hm_encode):
            self.PyEncodingName = pyencodingname  # Pythonでファイルを開く際にエンコードとして指定できる文字列( "cp932" や "utf8" など )
            self.MsCodePage = codepage  # マイクロソフトコードページの番号が入っている (932 や 65001 など)
            self.HmEncode = hm_encode      # 秀丸の encode としての値が入っている ( 1 や 6 など )


class _TFile:
    class _TStreamReader:

        def __init__(self, filepath, hm_encode=-1):
            try:
                if not os.path.exists(filepath):
                    raise FileNotFoundError

                self.Encoding = _TText._TEncoding("utf-8", 65001, 6)
                self.FilePath = filepath
            except:
                raise
        
        def __enter__(self):
            return self
            
        # 開いたファイルのテキストの取得
        def Read(self):
            try:
                if self.__filepath:
                    success, text = true, "aaaaaそうですね!"
                    if success:
                        return text
                    else:
                        raise IOError
            except:
                raise

        def Close(self):
            self.__encoding = None
            self.__filepath = None

        def __exit__(self, exception_type, exception_value, traceback):
            self.Close()


    #--------------------------------------------------
    # 編集中のテキスト全体
    def Open(self, filepath, hm_encode=-1):
        return _TFile._TStreamReader(filepath, hm_encode)
    
    def GetEncoding(self, filepath):
        try:
            if not os.path.exists(filepath):
                raise FileNotFoundError

            encoding_name, codepage, hm_encode = hidemaru.file.get_encodingfromfile(filepath)
            return _TText._TEncoding(encoding_name, codepage, hm_encode)
        except:
            raise
    

class _TEdit:

    #--------------------------------------------------
    # 編集中のテキスト全体
    TotalText = "abcdefg\nhijklmn\nopqrstu\nvwxyz"

    # 選択中のテキスト。通常選択もしくは行選択のみ（矩形選択は対象としない）
    SelectedText = "ijklm"

    # カーソルがある行のテキスト
    LineText = "nopqrstu"
    #--------------------------------------------------

    #--------------------------------------------------
    # カーソルの位置情報。linenoとcolumn（マクロのlinenoとcolumnと同じ値）
    #--------------------------------------------------
    class CursorPos:

        def __lineno(self):
            return 3;
        def __column(self):
            return 5;
        lineno = property(__lineno)
        column = property(__column)
    #--------------------------------------------------

    #--------------------------------------------------
    # マウスの位置に対応するカーソルの情報。
    # linenoとcolumn（マクロのlinenoとcolumnと同じ値） xとyは、win32 apiのマウス位置情報と同じ
    #--------------------------------------------------
    class MousePos:

        def __lineno(self):
            return 3;
        def __column(self):
            return 5;
        def __x(self):
            return 300;
        def __y(self):
            return 200;
        lineno = property(__lineno)
        column = property(__column)
        x = property(__x)
        y = property(__y)


class _TMacro:
    """
    秀丸マクロ関連のクラス
    """
    #--------------------------------------------------
    class _TVar:
        __map = {}
        """
        秀丸マクロ関連のうち、マクロシンボル（マクロ変数）を扱うクラス
        """

        def __getitem__(self, varname):
            return self.__map[varname]

        def __setitem__(self, varname, value):
            if not varname.startswith("#") and not varname.startswith("$"):
                hm.debuginfo(varname + " <= " + str(value) )
                hm.debuginfo("cant set attribute: フェイクデータ構築とみなします。本来のhmPyではこの代入は認められません。")

            self.__map[varname] = value
    #--------------------------------------------------

    #--------------------------------------------------
    class _TResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result, Message, ErrorMsg):
            self.Result = Result
            self.Message = Message
            self.Error = None
    #--------------------------------------------------

    #--------------------------------------------------
    def __init__(self):
        self.Var = _TMacro._TVar()
    #--------------------------------------------------

    #--------------------------------------------------
    # マクロの実行
    def Eval(self, expression_text):
        res, msg, errmsg = 1, "", ""
        ret = _TMacro._TResult(res, msg, errmsg)
        return ret
    #--------------------------------------------------


class _TOutputPane:
    """
    秀丸アウトプットパネル関連のクラス
    """
    # アウトプットパネルへの出力
    def Output(self, message):
        return 1
            
    # アウトプット枠情報の一時退避
    def Push(self):
        return 1

    # アウトプット枠情報の一時退避したものを復元
    def Push(self):
        return 1

    # アウトプット枠にメッセージを送る
    def SendMessage(self, command_id):
        return 1

    # アウトプット枠の基底ディレクトリを設定する
    def SetBaseDir(self, dirpath):
        return hidemaru.outputpane.setbasedir(dirpath)


class _THidemaru:
    """
    特定のカテゴリに所属しないようなもの
    """
    #--------------------------------------------------
    def __init__(self):
        self.File: _TFile = _TFile()
        self.Edit: _TEdit = _TEdit()
        self.Macro: _TMacro = _TMacro()
        self.OutputPane = _TOutputPane()
    #--------------------------------------------------

    #--------------------------------------------------
    # デバッグモニターに出力
    def debuginfo(self, obj):
        print(obj)
        return
    #--------------------------------------------------

    #--------------------------------------------------
    def __GetVersion(self):
        return 866.99

    # 秀丸のバージョンを 866.05 のような形で取得。
    # 「β5」なら少数部分が「05」のような形。
    # 正式版だと、866.99 のように「99」となる。
    version = property(__GetVersion)
    #--------------------------------------------------


hm = _THidemaru()
