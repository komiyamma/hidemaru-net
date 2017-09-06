IS_FAKE = True


class _TEdit:

    # 編集中のテキスト全体
    TotalText = "abcdefg\nhijklmn\nopqrstu\nvwxyz"

    # 選択中のテキスト。通常選択もしくは行選択のみ（矩形選択は対象としない）
    SelectedText = "ijklm"
    #--------------------------------------------------

    # カーソルがある行のテキスト
    LineText = "nopqrstu"
    #--------------------------------------------------

    #--------------------------------------------------
    class _TCursorPos:

        def __init__(self, lineno, column):
            self.lineno = lineno
            self.column = column
            
    def __GetCursorPos(self):
        lineno, column = 3, 5
        return _TEdit._TCursorPos(lineno, column)

    # カーソルの位置情報。linenoとcolumn（マクロのlinenoとcolumnと同じ値）
    CursorPos = property(__GetCursorPos)
    #--------------------------------------------------

    #--------------------------------------------------
    class _TMousePos:

        def __init__(self, lineno, column, x, y):
            self.lineno = lineno
            self.column = column
            self.x = x
            self.y = y

    def __GetMousePos(self):
        lineno, column, x, y = 3, 5, 300, 200
        return _TEdit._TMousePos(lineno, column, x, y)

    # マウスの位置に対応するカーソルの情報。
    # linenoとcolumn（マクロのlinenoとcolumnと同じ値） xとyは、win32 apiのマウス位置情報と同じ
    MousePos = property(__GetMousePos)
    #--------------------------------------------------


class _TMacro:
    """
    秀丸マクロ関連のクラス
    """
    #--------------------------------------------------
    class _TVar:
        """
        秀丸マクロ関連のうち、マクロシンボル（マクロ変数）を扱うクラス
        """

        def __getitem__(self, varname):
            return 999

        def __setitem__(self, varname, value):
            pass
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


class _TFakeHidemaru:
    
    FAKE = True
    
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
        return print(obj)
    #--------------------------------------------------

    # 秀丸のバージョンを 866.05 のような形で取得。
    # 「β5」なら少数部分が「05」のような形。
    # 正式版だと、866.99 のように「99」となる。
    version = 866.99
    #--------------------------------------------------


hm = _THidemaru()
