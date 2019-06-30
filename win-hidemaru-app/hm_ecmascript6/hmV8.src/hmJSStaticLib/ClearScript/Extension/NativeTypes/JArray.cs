﻿/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System.Collections.Generic;
using Microsoft.ClearScript.V8;

namespace hmV8DynamicNS.NativeTypes
{
    [NativeObject("Array")]
    public class JArray : IScriptNativeObject
    {
        private readonly object array;

        public JArray(V8ScriptEngine evaluator)
            : this(evaluator, null)
        {
        }

        public JArray(V8ScriptEngine evaluator, IEnumerable<object> items)
        {
            dynamic scriptObject = evaluator.Evaluate("[]");
            if (items == null)
            {
                return;
            }

            foreach (var item in items)
            {
                scriptObject.push(item);
            }

            array = scriptObject;
        }

        [NativeObject("Array")]
        public object GetScriptObject() => array;
    }
}
