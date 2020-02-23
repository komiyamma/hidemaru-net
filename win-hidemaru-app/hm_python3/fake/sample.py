from hmPython import *

if not "hidemaru" in globals():
    hm.Macro.Var["x"] = 33
    hm.Macro.Var["date"] = "aaaabbbb"
    hm.Macro.Var["lineno"] = 33

    hm.Edit.TotalText = "あああああああ"


hm.debuginfo(hm.version)

ret = hm.Macro.Eval(r'''
    message "OK";
''')


date = hm.Macro.Var['date']
hm.version = 3
hm.debuginfo(hm.version)


lineno = hm.Macro.Var['lineno']
hm.debuginfo(lineno, type(lineno))
