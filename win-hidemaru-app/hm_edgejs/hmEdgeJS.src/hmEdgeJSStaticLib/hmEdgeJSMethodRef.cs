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
    static Func<dynamic, Task<object>> refMacroFunction;
    static Func<dynamic, Task<object>> refMacroStatement;

    static Func<object, Task<object>> refFileGetHmEncode;
    static Func<object, Task<object>> refFileGetMsCodePage;
    static Func<object, Task<object>> refFileGetJsEncoding;

    static Func<dynamic, Task<object>> refFileReadAllText;

    static Func<object, Task<object>> refOutputPane_Output;
    static Func<object, Task<object>> refOutputPane_Push;
    static Func<object, Task<object>> refOutputPane_Pop;
    static Func<object, Task<object>> refOutputPane_Clear;
    static Func<object, Task<object>> refOutputPane_SendMessage;

    static Func<object, Task<object>> refExplorerPane_SetMode;
    static Func<object, Task<object>> refExplorerPane_GetMode;
    static Func<object, Task<object>> refExplorerPane_LoadProject;
    static Func<object, Task<object>> refExplorerPane_SaveProject;
    static Func<object, Task<object>> refExplorerPane_GetProject;
    static Func<object, Task<object>> refExplorerPane_SendMessage;
    static Func<object, Task<object>> refExplorerPane_GetUpdated;
    static Func<object, Task<object>> refExplorerPane_GetCurrentDir;

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

        refMacroFunction = (Func<dynamic, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Macro.Function((String)obj.FuncName, (Object[])(obj.Args));
            return ret;
        });

        refMacroStatement = (Func<dynamic, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.Macro.Statement((String)obj.FuncName, (Object[])(obj.Args));
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

        refOutputPane_Output = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.OutputPane.Output((String)obj);
            return ret;
        });

        refOutputPane_Push = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.OutputPane.Push();
            return ret;
        });

        refOutputPane_Pop = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.OutputPane.Pop();
            return ret;
        });

        refOutputPane_Clear = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.OutputPane.Clear();
            return ret;
        });

        refOutputPane_SendMessage = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.OutputPane.SendMessage((int)obj);
            return ret;
        });

        refExplorerPane_SetMode = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.SetMode((int)obj);
            return ret;
        });

        refExplorerPane_GetMode = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.GetMode();
            return ret;
        });

        refExplorerPane_LoadProject = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.LoadProject((String)obj);
            return ret;
        });

        refExplorerPane_SaveProject = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.SaveProject((String)obj);
            return ret;
        });

        refExplorerPane_GetProject = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.GetProject();
            return ret;
        });

        refExplorerPane_SendMessage = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.SendMessage((int)obj);
            return ret;
        });

        refExplorerPane_GetUpdated = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.GetUpdated();
            return ret;
        });

        refExplorerPane_GetCurrentDir = (Func<object, Task<object>>)(async (obj) =>
        {
            var ret = Hidemaru.ExplorerPane.GetCurrentDir();
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

                function _hm_refMacroFunction(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refMacroFunction(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refMacroStatement(obj) {
                    let ret = null;
                    let dumm = _TransRefObj.refMacroStatement(obj, function(error, result) { ret = result; } );
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

                function _hm_refOutputPane_Output(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refOutputPane_Output(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refOutputPane_Push(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refOutputPane_Push(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refOutputPane_Pop(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refOutputPane_Pop(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refOutputPane_Clear(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refOutputPane_Clear(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refOutputPane_SendMessage(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refOutputPane_SendMessage(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_SetMode(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refExplorerPane_SetMode(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_GetMode(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refExplorerPane_GetMode(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_LoadProject(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refExplorerPane_LoadProject(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_SaveProject(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refExplorerPane_SaveProject(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_GetUpdated(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refExplorerPane_GetUpdated(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_SendMessage(obj) {
                    let ret = 0;
                    let dumm = _TransRefObj.refExplorerPane_SendMessage(obj, function(error, result) { ret = result; } );
                    return ret;
                }

                function _hm_refExplorerPane_GetProject(obj) {
                    let text = """";
                    let dumm = _TransRefObj.refExplorerPane_GetProject(obj, function(error, result) { text = result; } );
                    return text;
                }

                function _hm_refExplorerPane_GetCurrentDir(obj) {
                    let text = """";
                    let dumm = _TransRefObj.refExplorerPane_GetCurrentDir(obj, function(error, result) { text = result; } );
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
                            Read: function() { return _hm_refFileReadAllText({FilePath:filepath, HmEncode:hm_encode}); },
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

                    static Function(funcname, ...args) {
                        return _hm_refMacroFunction( { FuncName:funcname, Args:args } );
                    }

                    static Statement(funcname, ...args) {
                        return _hm_refMacroStatement( { FuncName:funcname, Args:args } );
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

                class _hm_outputpane_ {
                    static Output(text) {
                        return _hm_refOutputPane_Output(text.toString());
                    }

                    static Push() {
                        return _hm_refOutputPane_Push();
                    }

                    static Pop() {
                        return _hm_refOutputPane_Pop();
                    }

                    static Clear() {
                        return _hm_refOutputPane_Clear();
                    }

                    static SendMessage(command_id) {
                        return _hm_refOutputPane_SendMessage(command_id);
                    }
                }

                class _hm_explorerpane_ {
                    static SetMode(mode) {
                        return _hm_refExplorerPane_SetMode(mode);
                    }

                    static GetMode() {
                        return _hm_refExplorerPane_GetMode();
                    }

                    static LoadProject(filepath) {
                        return _hm_refExplorerPane_LoadProject(filepath);
                    }

                    static SoadProject(filepath) {
                        return _hm_refExplorerPane_SoadProject(filepath);
                    }

                    static GetProject() {
                        return _hm_refExplorerPane_GetProject();
                    }

                    static SendMessage(command_id) {
                        return _hm_refExplorerPane_SendMessage(command_id);
                    }

                    static GetUpdated() {
                        return _hm_refExplorerPane_GetUpdated();
                    }

                    static GetCurrentDir() {
                        return _hm_refExplorerPane_GetCurrentDir();
                    }
                }

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

                    static get OutputPane() {
                        return _hm_outputpane_;
                    }

                    static get ExplorerPane() {
                        return _hm_explorerpane_;
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
                refMacroFunction = refMacroFunction,
                refMacroStatement = refMacroStatement,
                refManualResetEvent = refManualResetEvent,
                refFileGetHmEncode = refFileGetHmEncode,
                refFileGetMsCodePage = refFileGetMsCodePage,
                refFileGetJsEncoding = refFileGetJsEncoding,
                refFileReadAllText = refFileReadAllText,
                refOutputPane_Output = refOutputPane_Output,
                refOutputPane_Push = refOutputPane_Push,
                refOutputPane_Pop = refOutputPane_Pop,
                refOutputPane_Clear = refOutputPane_Clear,
                refOutputPane_SendMessage = refOutputPane_SendMessage,
                refExplorerPane_SetMode = refExplorerPane_SetMode,
                refExplorerPane_GetMode = refExplorerPane_GetMode,
                refExplorerPane_LoadProject = refExplorerPane_LoadProject,
                refExplorerPane_SaveProject = refExplorerPane_SaveProject,
                refExplorerPane_GetProject = refExplorerPane_GetProject,
                refExplorerPane_SendMessage = refExplorerPane_SendMessage,
                refExplorerPane_GetUpdated = refExplorerPane_GetUpdated,
                refExplorerPane_GetCurrentDir = refExplorerPane_GetCurrentDir,
            });

        }
        catch (Exception ex)
        {
            manualResetEvent.Set();
            OutputDebugStream(ex.ToString());
        }
    }
}


