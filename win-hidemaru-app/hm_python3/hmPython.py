import hidemaru


class _TEdit:

    def __GetTotalText(self):
        return hidemaru.edit.get_totaltext()

    def __SetTotalText(self, text):
        return hidemaru.edit.set_totaltext(text)

    TotalText = property(__GetTotalText, __SetTotalText)

    def __GetSelectedText(self):
        return hidemaru.edit.get_selectedtext()

    def __SetSelectedText(self, text):
        return hidemaru.edit.set_selectedtext(text)

    SelectedText = property(__GetSelectedText, __SetSelectedText)

    def __GetLineText(self):
        return hidemaru.edit.get_linetext()

    def __SetLineText(self, text):
        return hidemaru.edit.set_linetext(text)

    LineText = property(__GetLineText, __SetLineText)

    class _TCursorPos:

        def __init__(self, lineno, column):
            self.lineno = lineno
            self.column = column
            
    def __GetCursorPos(self):
        lineno, column = hidemaru.edit.get_cursorpos()
        return _TEdit._TCursorPos(lineno, column)

    CursorPos = property(__GetCursorPos)

    class _TMousePos:

        def __init__(self, lineno, column, x, y):
            self.lineno = lineno
            self.column = column
            self.x = x
            self.y = y

    def __GetMousePos(self):
        lineno, column, x, y = hidemaru.edit.get_mousepos()
        return _TEdit._TMousePos(lineno, column, x, y)

    MousePos = property(__GetMousePos)


class _TMacro:

    class _TVar:

       def __getitem__(self, varname):
           return hidemaru.macro.get_var(varname)

       def __setitem__(self, varname, value):
           return hidemaru.macro.set_var(varname, value)

    class _TResult:

        def __init__(self, Result, Message, ErrorMsg):
            self.Result = Result
            self.Message = Message
            if Result >= 1:
                self.Error = None
            else:
                self.Error = RuntimeError(ErrorMsg)

    def __init__(self):
        self.Var  = _TMacro._TVar()

    def Eval(self, expression_text):
        res, msg, errmsg = hidemaru.macro.do_eval(expression_text)
        ret = _TMacro._TResult(res, msg, errmsg)
        return ret




class _THidemaru:
    def __init__(self):
        self.Edit  = _TEdit()
        self.Macro = _TMacro()

    def debuginfo(self, text):
        return hidemaru.debug_info(text)

    def __GetVersion(self):
        return hidemaru.get_version()

    version = property(__GetVersion)




hm = _THidemaru()