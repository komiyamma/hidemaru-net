#-------------------- coding: utf-8 ---------------------------
# hmPython3 1.9.0.1用 ライブラリ
# Copyright (c) 2017-2021 Akitsugu Komiyama
# under the Apache License Version 2.0
#--------------------------------------------------------------

import hidemaru
import os
import random

__version__ = hidemaru.__version__

class _TText:
    class _TEncoding:

        def __init__(self, name, codepage, hm_encode):
            self.name = name  # Pythonでファイルを開く際にエンコードとして指定できる文字列( "cp932" や "utf8" など )
            self.codepage = codepage  # マイクロソフトコードページの番号が入っている (932 や 65001 など)
            self.hm_encode = hm_encode      # 秀丸の encode としての値が入っている ( 1 や 6 など )

class _TFile:
    """
    秀丸でファイル関連のクラス
    """

    class _TStreamReader:

        def __init__(self, filepath, hm_encode=-1):
            try:
                if not os.path.exists(filepath):
                    raise FileNotFoundError

                if hm_encode == -1:
                    encoding_name, codepage, hm_encode = hidemaru.file.get_encodingfromfile(filepath)
                else:
                    encoding_name, codepage, hm_encode = hidemaru.file.get_encodingfromhmencode(hm_encode)
                self.__encoding = _TText._TEncoding(encoding_name, codepage, hm_encode)
                self.__filepath = filepath
            except:
                raise

        def __enter__(self):
            return self

        def __GetEncoding(self):
            return self.__encoding

        # 開いたファイルのエンコーディング情報の取得
        Encoding = property(__GetEncoding)


        def __GetFilePath(self):
            return self.__filepath

        # ファイルパス
        FilePath = property(__GetFilePath)

        # 開いたファイルのテキストの取得
        def Read(self):
            try:
                if self.__filepath:
                    success, text = hidemaru.file.get_readalltext(self.__filepath, self.__encoding.hm_encode)
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

    # ファイルを開く。
    def Open(self, filepath, hm_encode=-1):
        return _TFile._TStreamReader(filepath, hm_encode)

    # 対象のファイルの encode (秀丸マクロの encode 変数の表を参照) を得る。
    def GetEncoding(self, filepath):
        try:
            if not os.path.exists(filepath):
                raise FileNotFoundError

            encoding_name, codepage, hm_encode = hidemaru.file.get_encodingfromfile(filepath)
            return _TText._TEncoding(encoding_name, codepage, hm_encode)
        except:
            raise


class _TEdit:
    """
    秀丸で編集中のドキュメント関連のクラス
    """

    def __GetFilePath(self):
        filepath = hidemaru.edit.get_filepath()
        if not filepath:
            return None
        else:
            return filepath
        
    # 編集中のファイルのパスを返す。ファイル名が決まってなければNoneを返す。
    FilePath = property(__GetFilePath)

    #--------------------------------------------------
    def __GetTotalText(self):
        return hidemaru.edit.get_totaltext()

    def __SetTotalText(self, text):
        return hidemaru.edit.set_totaltext(text)

    # 編集中のテキスト全体
    TotalText = property(__GetTotalText, __SetTotalText)
    #--------------------------------------------------

    #--------------------------------------------------
    def __GetSelectedText(self):
        return hidemaru.edit.get_selectedtext()

    def __SetSelectedText(self, text):
        return hidemaru.edit.set_selectedtext(text)

    # 選択中のテキスト。通常選択もしくは行選択のみ（矩形選択は対象としない）
    SelectedText = property(__GetSelectedText, __SetSelectedText)
    #--------------------------------------------------

    #--------------------------------------------------
    def __GetLineText(self):
        return hidemaru.edit.get_linetext()

    def __SetLineText(self, text):
        return hidemaru.edit.set_linetext(text)

    # カーソルがある行のテキスト
    LineText = property(__GetLineText, __SetLineText)
    #--------------------------------------------------

    #--------------------------------------------------
    class _TCursorPos:

        def __init__(self, lineno, column):
            self.lineno = lineno
            self.column = column

    def __GetCursorPos(self):
        lineno, column = hidemaru.edit.get_cursorpos()
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
        lineno, column, x, y = hidemaru.edit.get_mousepos()
        return _TEdit._TMousePos(lineno, column, x, y)

    # マウスの位置に対応するカーソルの情報。
    # linenoとcolumn（マクロのlinenoとcolumnと同じ値） xとyは、win32 apiのマウス位置情報と同じ
    MousePos = property(__GetMousePos)
    #--------------------------------------------------


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
            value_list = []
            type_list = []
            for arg in args:
                if type(arg) is int or type(arg) is bool or type(arg) is float:
                    value_list.append(int(arg))
                    type_list.append('int')
                elif type(arg) is list or type(arg) is tuple:
                    if all(type(e) is str for e in arg):
                        value_list.append(tuple(arg))
                        type_list.append('array_str')
                    elif all((type(e) is int or type(e) is float or type(e) is bool) for e in arg):
                        modify_list = [int(e) for e in arg]
                        value_list.append(tuple(modify_list))
                        type_list.append('array_int')
                        
                else:
                    value_list.append(str(arg))
                    type_list.append('str')
            
            res, msg, errmsg, args = hidemaru.macro.do_statement(statement_name, tuple(value_list), tuple(type_list))
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
            value_list = []
            type_list = []
            for arg in args:
                if type(arg) is int or type(arg) is bool or type(arg) is float:
                    value_list.append(int(arg))
                    type_list.append('int')
                else:
                    value_list.append(str(arg))
                    type_list.append('str')
            
            res, msg, errmsg, args = hidemaru.macro.do_function(function_name, tuple(value_list), tuple(type_list))
            ret = _TMacro._TFunctionResult(res, msg, errmsg, args)
            
            return ret

    #--------------------------------------------------
    class _TVar:
        """
        秀丸マクロ関連のうち、マクロシンボル（マクロ変数）を扱うクラス
        """

        def __getitem__(self, varname):
            return hidemaru.macro.get_var(varname)

        def __setitem__(self, varname, value):
            if not varname.startswith("#") and not varname.startswith("$"):
                hidemaru.debug_info("指定のマクロ変数への値の代入は出来ません: " + varname)
                hidemaru.macro.set_var(varname, value)
                raise NameError(varname)
            else:
                return hidemaru.macro.set_var(varname, value)

        def __getattr__(self, varname):
            return hidemaru.macro.get_var(varname)

    #--------------------------------------------------

    #--------------------------------------------------
    class _TResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result, Message, ErrorMsg):
            self.Result = Result
            self.Message = Message
            if Result >= 1:
                self.Error = None
            else:
                self.Error = RuntimeError(ErrorMsg)
    #--------------------------------------------------
    class _TStatementResult:
        """
        秀丸マクロ関連のうち、マクロ実行結果情報を扱うクラス
        """

        def __init__(self, Result, Message, ErrorMsg, Args):
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

        def __init__(self, Result, Message, ErrorMsg, Args):
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
    def Eval(self, expression_text):
        res, msg, errmsg = hidemaru.macro.do_eval(expression_text)
        ret = _TMacro._TResult(res, msg, errmsg)
        return ret
    #--------------------------------------------------

class _TOutputPane:
    """
    秀丸アウトプットペイン関連のクラス
    """
    # アウトプット枠への出力
    def Output(self, obj):
        return hidemaru.outputpane.output(obj)
            
    # アウトプット枠情報の一時退避
    def Push(self):
        return hidemaru.outputpane.push()

    # アウトプット枠情報の一時退避したものを復元
    def Push(self):
        return hidemaru.outputpane.pop()

    # アウトプット枠のクリア
    def Clear(self):
        return hidemaru.outputpane.clear()

    # アウトプット枠にメッセージを送る
    def SendMessage(self, command_id):
        return hidemaru.outputpane.sendmessage(command_id)

    # アウトプット枠の基底ディレクトリを設定する
    def SetBaseDir(self, dirpath):
        return hidemaru.outputpane.setbasedir(dirpath)


class _TExplorerPane:
    """
    秀丸ファイルマネージャペイン関連のクラス
    """
    # ファイルマネージャ枠のモードの設定
    def SetMode(self, mode):
        return hidemaru.explorerpane.setmode(mode)

    # ファイルマネージャ枠のモードの取得
    def GetMode(self):
        return hidemaru.explorerpane.getmode()

    # ファイルマネージャ枠に指定のファイルのプロジェクトを読み込む
    def LoadProject(self, filepath):
        return hidemaru.explorerpane.loadproject(filepath)

    # ファイルマネージャ枠のプロジェクトを指定ファイルに保存
    def SaveProject(self, filepath):
        return hidemaru.explorerpane.saveproject(filepath)

    # ファイルマネージャ枠が「プロジェクト」表示のとき、更新された状態であるかどうかを返します
    def GetUpdated(self):
        return hidemaru.explorerpane.getupdated()

    # ファイルマネージャ枠にメッセージを送る
    def SendMessage(self, command_id):
        return hidemaru.explorerpane.sendmessage(command_id)

    # ファイルマネージャ枠にプロジェクトを読み込んでいるならば、そのファイルパスを取得する(読み込んでいなければNoneが返る)
    def GetProject(self):
        filepath = hidemaru.explorerpane.getproject()
        if not filepath:
            return None
        else:
            return filepath

    # ファイルマネージャ枠のカレントディレクトリを返す
    def GetCurrentDir(self):
        dirpath = hidemaru.explorerpane.getcurrentdir()
        if not dirpath:
            return None
        else:
            return dirpath


class _THidemaru:
    """
    特定のカテゴリに所属しないようなもの
    """
    #--------------------------------------------------
    def __init__(self):
        self.File = _TFile()
        self.Edit = _TEdit()
        self.Macro = _TMacro()
        self.OutputPane = _TOutputPane()
        self.ExplorerPane = _TExplorerPane()
    #--------------------------------------------------

    #--------------------------------------------------
    # デバッグモニターに出力
    def debuginfo(self, obj):
        return hidemaru.debug_info(obj)
    #--------------------------------------------------

    #--------------------------------------------------
    def __GetVersion(self):
        return hidemaru.get_version()

    # 秀丸のバージョンを 866.05 のような形で取得。
    # 「β5」なら少数部分が「05」のような形。
    # 正式版だと、866.99 のように「99」となる。
    version = property(__GetVersion)
    #--------------------------------------------------


hm = _THidemaru()