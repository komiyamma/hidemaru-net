# coding: utf-8

#--------------------------------------------------------------
# hmPy 1.5.6.1用 フェイクライブラリ
# Copyright (c) 2016-2018 Akitsugu Komiyama
# under the Apache License Version 2.0
#--------------------------------------------------------------


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
                hm.debuginfo("cant set attribute: ファイクデータ構築とみなします。本来のhmPython3ではこの代入は認められません。")

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
