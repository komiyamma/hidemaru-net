#-------------------- coding: utf-8 ---------------------------
# hmPython3 1.9.0.1用 フェイクライブラリ
# Copyright (c) 2017-2021 Akitsugu Komiyama
# under the Apache License Version 2.0
#--------------------------------------------------------------
import os

__version__ = 1.901

class _TText:
    class _TEncoding:
        def __init__(self, name: str, codepage: str, hm_encode):
            self.name: str = name  # Pythonでファイルを開く際にエンコードとして指定できる文字列( "cp932" や "utf8" など )
            self.codepage: int = codepage  # マイクロソフトコードページの番号が入っている (932 や 65001 など)
            self.hm_encode: int = hm_encode      # 秀丸の encode としての値が入っている ( 1 や 6 など )


class _TFile:
    class _TStreamReader:

        def __init__(self, filepath: str, hm_encode: int=-1):
            try:
                if not os.path.exists(filepath):
                    raise FileNotFoundError

                self.Encoding: _TText._TEncoding = _TText._TEncoding("utf-8", 65001, 6)
                self.FilePath: str = filepath
            except:
                raise

        def __enter__(self):
            return self

        # 開いたファイルのテキストの取得
        def Read(self) -> str:
            try:
                if self.__filepath:
                    success, text = True, "aaaaaそうですね!"
                    if success:
                        return text
                    else:
                        raise IOError
            except:
                raise

        def Close(self) -> None:
            self.__encoding = None
            self.__filepath = None

        def __exit__(self, exception_type, exception_value, traceback):
            self.Close()


    #--------------------------------------------------
    # 編集中のテキスト全体
    def Open(self, filepath: str, hm_encode: int=-1) -> _TStreamReader:
        return _TStreamReader(filepath, hm_encode)

    def GetEncoding(self, filepath: str) -> _TText._TEncoding:
        try:
            if not os.path.exists(filepath):
                raise FileNotFoundError

            encoding_name, codepage, hm_encode = "utf-8", 65001, 6
            return _TText._TEncoding(encoding_name, codepage, hm_encode)
        except:
            raise



class _TEdit:

    FilePath: str = r"C:\test\test.txt"
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
    class _TAsStatement:
        """
        秀丸マクロ関連のうち、括弧がなく値が変えられない秀丸組み込みの(関数のように使う)文のラップを表すクラス
        """
        def __getattr__(self, statement_name):
            return lambda *args: self.closure(statement_name, *args)

        def __call__(self, statement_name, *args):
            return self.closure(statement_name, *args)

        def closure(self, statement_name, *args):
            (res, msg, errmsg, args) = (1, "", None, args)
            ret = _TMacro._TStatementResult(res, msg, errmsg, args)
            
            return ret
            
    class _TAsFunction:
        """
        秀丸マクロ関連のうち、括弧があり値が返る秀丸組み込みの関数のラップを表すクラス
        """
        def __getattr__(self, function_name):
            return lambda *args: self.closure(function_name, *args)

        def __call__(self, statement_name, *args):
            return self.closure(statement_name, *args)

        def closure(self, function_name, *args):
            (res, msg, errmsg, args) = (1, "", None, args)
            ret = _TMacro._TFunctionResult(res, msg, errmsg, args)
            
            return ret
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

        def __getattr__(self, varname: str):
            return self.__map[varname]
    #--------------------------------------------------

    #--------------------------------------------------
    class _TResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result: int, Message: str, ErrorMsg: str):
            self.Result: int = Result
            self.Message: str = Message
            self.Error: str = None
    #--------------------------------------------------
    class _TStatementResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result: int, Message: str, ErrorMsg: str, Args):
            self.Result = Result
            self.Message = Message
            self.Args = tuple(Args)
            if Result >= 1:
                self.Error = None
            else:
                self.Error = RuntimeError(ErrorMsg)
    #--------------------------------------------------
    class _TFunctionResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result, Message: str, ErrorMsg: str, Args):
            self.Result = Result
            self.Message = Message
            self.Args = tuple(Args)
            if Result != None:
                self.Error = None
            else:
                self.Error = RuntimeError(ErrorMsg)

    #--------------------------------------------------
    def __init__(self):
        self.Var = _TMacro._TVar()
        self.Function = _TMacro._TAsFunction();
        self.Statement = _TMacro._TAsStatement();
    #--------------------------------------------------

    #--------------------------------------------------
    # マクロの実行
    def Eval(self, expression_text: str) -> _TResult:
        res: int = 1
        msg: str = ""
        errmsg: str = ""
        ret: _TMacro._TResult = _TMacro._TResult(res, msg, errmsg)
        return ret
    #--------------------------------------------------


class _TOutputPane:
    """
    秀丸アウトプットパネル関連のクラス
    """
    # アウトプットパネルへの出力
    def Output(self, obj) -> int:
        print(obj, end='')
        return 1
            
    # アウトプット枠情報の一時退避
    def Push(self) -> int:
        return 1

    # アウトプット枠情報の一時退避したものを復元
    def Push(self) -> int:
        return 1

    # アウトプット枠のクリア
    def Clear(self) -> int:
        return 0

    # アウトプット枠にメッセージを送る
    def SendMessage(self, command_id: int) -> int:
        return 1

    # アウトプット枠の基底ディレクトリを設定する
    def SetBaseDir(self, dirpath) -> int:
        return 1


class _TExplorerPane:
    """
    秀丸ファイルマネージャペイン関連のクラス
    """
    # ファイルマネージャ枠のモードの設定
    def SetMode(self, mode: int) -> int:
        return 1

    # ファイルマネージャ枠のモードの取得
    def GetMode(self) -> int:
        return 1

    # ファイルマネージャ枠に指定のファイルのプロジェクトを読み込む
    def LoadProject(self, filepath: str) -> int:
        return 1

    # ファイルマネージャ枠のプロジェクトを指定ファイルに保存
    def SaveProject(self, filepath: str) -> int:
        return 1

    # ファイルマネージャ枠が「プロジェクト」表示のとき、更新された状態であるかどうかを返します
    def GetUpdated(self) -> int:
        return 0

    # ファイルマネージャ枠にメッセージを送る
    def SendMessage(self, command_id) -> int:
        return 0

    # ファイルマネージャ枠にプロジェクトを読み込んでいるならば、そのファイルパスを取得する(読み込んでいなければNoneが返る)
    def GetProject(self) -> str:
        return None

    # ファイルマネージャ枠のカレントディレクトリを返す
    def GetCurrentDir(self) -> str:
        return os.getcwd()


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
        self.ExplorerPane = _TExplorerPane()
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


hm: _THidemaru = _THidemaru()
