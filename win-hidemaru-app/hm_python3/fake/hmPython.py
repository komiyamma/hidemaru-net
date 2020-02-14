#--------------------------------------------------------------
# hmPython3 1.5.8.1用 フェイクライブラリ
# Copyright (c) 2017-2019 Akitsugu Komiyama
# under the Apache License Version 2.0
#--------------------------------------------------------------

class _TFile:
    """
    秀丸でファイル関連のクラス
    """
    class _TEncoding:

        def __init__(self, py_encoding, ms_codepage, hm_encode):
            self.py_encoding = py_encoding  # Pythonでファイルを開く際にエンコードとして指定できる文字列( "cp932" や "utf8" など )
            self.ms_codepage = ms_codepage  # マイクロソフトコードページの番号が入っている (932 や 65001 など)
            self.hm_encode = hm_encode      # 秀丸の encode としての値が入っている ( 1 や 6 など )
    
    # 対象のファイルの encode (秀丸マクロの encode 変数の表を参照) を得る。
    def GetEncode(self, filefullpath):
        py_encoding, ms_codepage, hm_encode = "utf_8", 65001, 6
        return _TFile._TEncoding(py_encoding, ms_codepage, hm_encode)


class _TEdit:

    #--------------------------------------------------
    # 編集中のテキスト全体
    TotalText: str = "abcdefg\nhijklmn\nopqrstu\nvwxyz"

    # 選択中のテキスト。通常選択もしくは行選択のみ（矩形選択は対象としない）
    SelectedText: str = "ijklm"
    #--------------------------------------------------

    # カーソルがある行のテキスト
    LineText: str = "nopqrstu"
    #--------------------------------------------------

    #--------------------------------------------------
    # カーソルの位置情報。linenoとcolumn（マクロのlinenoとcolumnと同じ値）
    #--------------------------------------------------
    class CursorPos:

        def __lineno(self) -> int:
            return 3
        def __column(self) -> int:
            return 5
        lineno = property(__lineno)
        column = property(__column)
    #--------------------------------------------------

    #--------------------------------------------------
    # マウスの位置に対応するカーソルの情報。
    # linenoとcolumn（マクロのlinenoとcolumnと同じ値） xとyは、win32 apiのマウス位置情報と同じ
    #--------------------------------------------------
    class MousePos:

        def __lineno(self) -> int:
            return 3
        def __column(self) -> int:
            return 5
        def __x(self) -> int:
            return 300
        def __y(self) -> int:
            return 200
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
        __map: dict = {}
        """
        秀丸マクロ関連のうち、マクロシンボル（マクロ変数）を扱うクラス
        """

        def __getitem__(self, varname: str):
            return self.__map[varname]

        def __setitem__(self, varname: str, value):
            if not varname.startswith("#") and not varname.startswith("$"):
                hm.debuginfo(varname + " <= " + str(value) )
                hm.debuginfo("cant set attribute: フェイクデータ構築とみなします。本来のhmPython3ではこの代入は認められません。")

            self.__map[varname] = value
    #--------------------------------------------------

    #--------------------------------------------------
    class _TResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result, Message, ErrorMsg):
            self.Result: int = Result
            self.Message: str = Message
            self.Error: str = None
    #--------------------------------------------------

    #--------------------------------------------------
    def __init__(self):
        self.Var = _TMacro._TVar()
    #--------------------------------------------------

    #--------------------------------------------------
    # マクロの実行
    def Eval(self, expression_text: str):
        res, msg, errmsg = 1, "", ""
        ret = _TMacro._TResult(res, msg, errmsg)
        return ret
    #--------------------------------------------------


class _THidemaru:
    """
    特定のカテゴリに所属しないようなもの
    """
    #--------------------------------------------------
    def __init__(self):
        self.Edit = _TEdit()
        self.Macro = _TMacro()
    #--------------------------------------------------

    #--------------------------------------------------
    # デバッグモニターに出力
    def debuginfo(self, obj) -> None:
        print(obj)
        return
    #--------------------------------------------------

    #--------------------------------------------------
    def __GetVersion(self) -> float:
        return 866.99

    # 秀丸のバージョンを 866.05 のような形で取得。
    # 「β5」なら少数部分が「05」のような形。
    # 正式版だと、866.99 のように「99」となる。
    version = property(__GetVersion)
    #--------------------------------------------------


hm = _THidemaru()
