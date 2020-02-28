/* 
 * Copyright (c) 2016-2018 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using EdgeJs;
using System;
using System.Threading;
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
    static Func<dynamic, Task<object>> refMacroSetVar;

    static Func<object, Task<object>> refFileGetHmEncode;
    static Func<object, Task<object>> refFileGetMsCodePage;
    static Func<object, Task<object>> refFileGetJsEncoding;

    static Func<dynamic, Task<object>> refFileReadAllText;

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

        refMacroSetVar = (Func<dynamic, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Macro.SetVar((String)obj.VarName, obj.VarValue);
            return ret;
        });

        refFileGetHmEncode = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.File.GetHmEncode((String)obj);
            return ret;
        });

        refFileGetMsCodePage = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.File.GetMsCodePage((int)obj);
            return ret;
        });

        refFileGetJsEncoding = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.File.GetJsEncoding((int)obj);
            return ret;
        });

        refFileReadAllText = (Func<dynamic, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.File.ReadAllText((String)obj.FilePath, (int)obj.HmEncode);
            return ret;
        });
    }

    public static async Task InitExpression(string filename)
    {

        try
        {
            var refManualResetEvent = (Func<object, Task<object>>)(async (obj) =>
            {
                manualResetEvent.Set();
                return true;
            });

            var func = Edge.Func(@"

                var _TransRefObj = null;

                function R(text){
                    return text.toString().match(/\/\*([\s\S]*)\*\//)[1].toString();
                }

                global.R = R;

                function _hm_refManualResetEvent(obj) {
                    let dumm = _TransRefObj.refManualResetEvent(obj, function(error, result) { ; } );
                }

                function _hm_refGetVersion(obj) {
                    let ver = 0;
                    let dumm = _TransRefObj.refGetVersion(obj, function(error, result) { ver = result; } );
                    return ver;
                }

                function _hm_refDebugInfo(...args) {
                    let format = require('util').format;
                    let joined = format(...args);
                    _TransRefObj.refDebugInfo(joined);
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

                function _hm_refMacroSetVar(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refMacroSetVar(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refFileGetHmEncode(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refFileGetHmEncode(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refFileGetMsCodePage(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refFileGetMsCodePage(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refFileGetJsEncoding(obj) {
                    let ret = """";
                    let dumm = _TransRefObj.refFileGetJsEncoding(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refFileReadAllText(obj) {
                    let text = """";
                    let dumm = _TransRefObj.refFileReadAllText(obj, function(error, result) { text = result; } );
                    return text;
                }

                class _hm_file_ {
                    static Open(filepath, hm_encode_override = -1) {

                        let hm_encode = -1;
                        if (hm_encode_override == -1) {
                            hm_encode = _hm_refFileGetHmEncode(filepath);
                        } else {
                            hm_encode = hm_encode_override;
                        }
                        let codepage = _hm_refFileGetMsCodePage(hm_encode);
                        let name = _hm_refFileGetJsEncoding(hm_encode);
                        let encoding = { HmEncode: hm_encode, MsCodePage: codepage, JsEncodingName: name };

                        return {
                            Encoding: encoding,
                            FilePath: filepath,
                            Read: function() { _hm_refFileReadAllText({FilePath:filepath, HmEncode:hm_encode}); },
                            Close: function() { this.Encoding = null; this.FilePath = null; }
                        };
                    }

                    static GetEncoding(filepath) {
                        let hm_encode = _hm_refFileGetHmEncode(filepath);
                        let codepage = _hm_refFileGetMsCodePage(hm_encode);
                        let name = _hm_refFileGetJsEncoding(hm_encode);
                        return { HmEncode: hm_encode, MsCodePage: codepage, JsEncodingName: name };
                    }

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
                            _hm_refMacroSetVar( {key:prop, value:val} );
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
                            return _hm_refMacroSetVar( {VarName:args[0], VarValue:args[1]} );
                        }
                        else if (args.length == 1 ) {
                            return _hm_refMacroGetVar(args[0]);
                        }
                    },
                    get(target, prop, receiver) {
                        return _hm_refMacroGetVar(prop);
                    },
                    set(target, prop, val, receiver) {
                        return _hm_refMacroSetVar( {VarName:prop, VarValue:val} );
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

                    static get File() {
                        return _hm_file_;
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
                    try {
                        // まず値全体を代入
                        _TransRefObj = data;

                        // モジュールパスの追加
                        // global.module.paths.push( hm.Macro.Var[""currentmacrodirectory""] );

                        // 読み込み(成功するとメモリにキャッシュされるので注意。対策するため次のキャッシュ削除がある)
                        require(data.filename);

                    } catch(err) {

                        hm.debuginfo(err);

                    } finally {

                        // 複数回実行しても確実に実行されるように、requireのキャッシュは毎回削除しておく。
                        Object.keys(require.cache).forEach(function(key) { delete require.cache[key] }) 

                        // 決まり文句
                        callback(null, 'hmEdgeJS:' + data);

                        // 最後までやったという実績
                        _hm_refManualResetEvent();
                     }
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
                refMacroSetVar = refMacroSetVar,
                refManualResetEvent = refManualResetEvent,
                refFileGetHmEncode = refFileGetHmEncode,
                refFileGetMsCodePage = refFileGetMsCodePage,
                refFileGetJsEncoding = refFileGetJsEncoding,
                refFileReadAllText = refFileReadAllText
            });

        }
        catch (Exception ex)
        {
            manualResetEvent.Set();
            OutputDebugStream(ex.ToString());
        }
    }
}


