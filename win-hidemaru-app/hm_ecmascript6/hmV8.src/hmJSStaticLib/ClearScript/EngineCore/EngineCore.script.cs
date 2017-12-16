/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;
using Microsoft.ClearScript;
using hmV8DynamicNS.Extensions;

namespace hmV8DynamicNS
{
    public partial class V8EngineCore
    {
        [ScriptMember("eval")]
        public bool ScriptEvaluate()
        {
            try
            {
                engine.Execute("coreEval", @"EngineInternal.evalResult = eval(EngineInternal.evalCode)");
                return true;
            }
            catch (Exception ex)
            {
                engine.Script.EngineInternal.evalError = ex.GetScriptStack();
                engine.Script.EngineInternal.lastError = ex;
                return false;
            }
        }

        [ScriptMember("sleep")]
        public object Sleep(bool startRepl = true)
        {
            Sleeping = true;
            Module stdIn;
            return NativeModules.TryGetValue("stdin", out stdIn) ? stdIn.Exports : null;
        }
    }
}
