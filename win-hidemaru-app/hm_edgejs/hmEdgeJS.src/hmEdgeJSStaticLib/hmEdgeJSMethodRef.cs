/* 
 * Copyright (c) 2016-2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using EdgeJs;
using System;
using System.Threading.Tasks;

// ★クラス実装内のメソッドの中でdynamic型を利用したもの。これを直接利用しないのは、内部でdynamic型を利用していると、クラスに自動的にメソッドが追加されてしまい、C++とはヘッダのメソッドの個数が合わなくなりリンクできなくなるため。
public sealed partial class hmEdgeJSDynamicLib
{
    static Func<object, Task<object>> refDebugInfo;
    static Func<object, Task<object>> refGetVersion;
    static Func<object, Task<object>> refEditGetTotalText;
    static Func<object, Task<object>> refEditSetTotalText;
    static Func<object, Task<object>> refEditGetSelectedText;
    static Func<object, Task<object>> refEditSetSelectedText;
    static Func<object, Task<object>> refEditGetLineText;
    static Func<object, Task<object>> refEditSetLineText;

    static Func<object, Task<object>> refEditGetCursorPos;
    static Func<object, Task<object>> refEditGetMousePos;

    static Func<object, Task<object>> refMacroEval;
    static Func<object, Task<object>> refMacroGetVar;

    static string strSetVarName = "";
    static Func<object, Task<object>> refMacroSetVarName;
    static Func<object, Task<object>> refMacroSetVarValue;


    static void InitMethodReference()
    {
        refDebugInfo = (Func<object, Task<object>>)(async (obj) =>
        {
            Hidemaru.debuginfo(obj);
            return true;
        });

        refGetVersion = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.version;
            System.Diagnostics.Trace.WriteLine(ret);
            return ret;
        });

        refEditGetTotalText = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Edit.GetTotalText();
            return ret;
        });

        refEditSetTotalText = (Func<object, Task<object>>)(async (obj) =>
        {
            Hidemaru.Edit.SetTotalText((String)obj);
            return true;
        });

        refEditGetSelectedText = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Edit.GetSelectedText();
            return ret;
        });

        refEditSetSelectedText = (Func<object, Task<object>>)(async (obj) =>
        {
            Hidemaru.Edit.SetSelectedText((String)obj);
            return true;
        });

        refEditGetLineText = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Edit.GetLineText();
            return ret;
        });

        refEditSetLineText = (Func<object, Task<object>>)(async (obj) =>
        {
            Hidemaru.Edit.SetLineText((String)obj);
            return true;
        });

        refEditGetCursorPos = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Edit.GetCursorPos();
            return ret;
        });

        refEditGetMousePos = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Edit.GetCursorPosFromMousePos();
            return ret;
        });

        refMacroEval = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Macro.Eval(obj);
            return ret;
        });

        refMacroGetVar = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Macro.GetVar((String)obj);
            return ret;
        });

        refMacroSetVarName = (Func<object, Task<object>>)(async (obj) =>
        {
            strSetVarName = (String)obj;
            return true;
        });

        refMacroSetVarValue = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Macro.SetVar(strSetVarName, obj);
            return ret;
        });

    }

    public static async Task InitExpression(string filename)
    {
        try
        {
            var func = Edge.Func(@"

                var _TransRefObj = null;

                function R(text){
                    return text.toString().match(/\/\*([\s\S]*)\*\//)[1].toString();
                }

                global.R = R;

                function _hm_refGetVersion(obj) {
                    let ver = 0;
                    let dumm = _TransRefObj.refGetVersion(obj, function(error, result) { ver = result; } );
                    return ver;
                }

                function _hm_refDebugInfo(...args) {
                    let list = [];
                    for(let o of args) {
　　　　　　　　　　　  if (o == undefined) {
                            list.push(""undefined"");
                        }
　　　　　　　　　　　  else if (o == null) {
                            list.push(""null"");
                        }
                        else {
                            var stringify = JSON.stringify(o);

                            // stringify不可能だが、toStringは可能
                            if (stringify == undefined && o.toString && typeof(o.toString) == ""function"" ) {
                                list.push(o.toString());

                            // stringify可能だった
                            } else {
                                list.push(stringify);
                            }
                        }
                    }
                    let joined = list.join(' ');
                    _TransRefObj.refDebugInfo(joined);
                    // console.log(args);
                }

                function _hm_refEditGetTotalText(obj) {
                    let text = """";
                    let dumm = _TransRefObj.refEditGetTotalText(obj, function(error, result) { text = result; } );
                    return text;
                }

                function _hm_refEditSetTotalText(obj) {
                    let ret = false;
                    let dumm = _TransRefObj.refEditSetTotalText(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refEditGetSelectedText(obj) {
                    let text = """";
                    let dumm = _TransRefObj.refEditGetSelectedText(obj, function(error, result) { text = result; } );
                    return text;
                }

                function _hm_refEditSetSelectedText(obj) {
                    let ret = false;
                    let dumm = _TransRefObj.refEditSetSelectedText(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refEditGetLineText(obj) {
                    let text = """";
                    let dumm = _TransRefObj.refEditGetLineText(obj, function(error, result) { text = result; } );
                    return text;
                }

                function _hm_refEditSetLineText(obj) {
                    let ret = false;
                    let dumm = _TransRefObj.refEditSetLineText(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refEditGetCursorPos(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refEditGetCursorPos(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refEditGetMousePos(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refEditGetMousePos(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refMacroEval(expression) {
                    let ret = null;
                    let obj = null;
                    if ( typeof(expression) == ""function"" ) {
                        obj = R(expression);
                    } else {
                        obj = expression;
                    }
                    let dumm = _TransRefObj.refMacroEval(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refMacroGetVar(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refMacroGetVar(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refMacroSetVarName(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refMacroSetVarName(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refMacroSetVarValue(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refMacroSetVarValue(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                class _hm_edit_ {
                    static get TotalText() {
                        return _hm_refEditGetTotalText();
                    }

                    static set TotalText(text) {
                        _hm_refEditSetTotalText(text);
                    }

                    static get SelectedText() {
                        return _hm_refEditGetSelectedText();
                    }

                    static set SelectedText(text) {
                        _hm_refEditSetSelectedText(text);
                    }

                    static get LineText() {
                        return _hm_refEditGetLineText();
                    }

                    static set LineText(text) {
                        _hm_refEditSetLineText(text);
                    }

                    static get CursorPos() {
                        return _hm_refEditGetCursorPos();
                    }

                    static get MousePos() {
                        return _hm_refEditGetMousePos();
                    }

                }

                class _hm_macro_ {
                    static Eval(expression) {
                        return _hm_refMacroEval(expression);
                    }
/*
                    static Var(key, value) {
                        if (value == null) {
                            return _hm_refMacroGetVar(key);
                        } else {
                            _hm_refMacroSetVarName(key);
                            return _hm_refMacroSetVarValue(value);
                        }
                    }
*/
                }

                // hmJSやhmV8と同じ。
                // hm.Macro.Var('key') と hm.Macro.Var['key']
                // hm.Macro.Var('key', value)とhm.Macro.Var['key'] = value
                // の両方を実現するため。
                _hm_macro_.Var = new Proxy(()=>{}, {
                    apply: function(target, that, args) {
                        if (args.length > 1 ) {
                            _hm_refMacroSetVarName(args[0]);
                            return _hm_refMacroSetVarValue(args[1]);
                        }
                        else if (args.length == 1 ) {
                            return _hm_refMacroGetVar(args[0]);
                        }
                    },
                    get(target, prop, receiver) {
                        return _hm_refMacroGetVar(prop);
                    },
                    set(target, prop, val, receiver) {
                        _hm_refMacroSetVarName(prop);
                        return _hm_refMacroSetVarValue(val);
                    }
                }
                );

                class hm {
                    static get version() {
                        return _hm_refGetVersion();
                    }

                    static debuginfo(...args) {
                        return _hm_refDebugInfo(...args);
                    }

                    static get Edit() {
                        return _hm_edit_;
                    }

                    static get Macro() {
                        return _hm_macro_;
                    }
                };
        
                console.log = hm.debuginfo
                
                global.hm = hm;

                return function(data, callback) {

                    // まず値全体を代入
                    _TransRefObj = data;

                    // モジュールパスの追加
                    // global.module.paths.push( hm.Macro.Var[""currentmacrodirectory""] );

                    // 読み込み(成功するとメモリにキャッシュされるので注意。対策するため次のキャッシュ削除がある)
                    require(data.filename);

                    // 複数回実行しても確実に実行されるように、requireのキャッシュは毎回削除しておく。
                    Object.keys(require.cache).forEach(function(key) { delete require.cache[key] }) 

                    // 決まり文句
                    callback(null, 'hmEdgeJS:' + data);
                }

            ");

            var ret = await func(new
            {
                port = 8080,
                filename = filename,
                refDebugInfo = refDebugInfo,
                refGetVersion = refGetVersion,
                refEditGetTotalText = refEditGetTotalText,
                refEditSetTotalText = refEditSetTotalText,
                refEditGetSelectedText = refEditGetSelectedText,
                refEditSetSelectedText = refEditSetSelectedText,
                refEditGetLineText = refEditGetLineText,
                refEditSetLineText = refEditSetLineText,
                refEditGetCursorPos = refEditGetCursorPos,
                refEditGetMousePos = refEditGetMousePos,
                refMacroEval = refMacroEval,
                refMacroGetVar = refMacroGetVar,
                refMacroSetVarName = refMacroSetVarName,
                refMacroSetVarValue = refMacroSetVarValue
            });

        }
        catch (Exception ex)
        {
            OutputDebugStream(ex.ToString());
        }
    }
}

