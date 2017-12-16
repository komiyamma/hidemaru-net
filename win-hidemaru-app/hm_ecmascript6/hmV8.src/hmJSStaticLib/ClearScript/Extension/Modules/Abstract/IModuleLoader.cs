﻿/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System.Collections.Generic;
using Microsoft.ClearScript.V8;

namespace hmV8DynamicNS
{
    public interface IModuleLoader
    {
        V8ScriptEngine Evaluator { get; }

        Dictionary<string, Module> NativeModules { get; }

        Dictionary<string, Module> LoadedModules { get; }

        Dictionary<string, IModuleCompiler> Compilers { get; }

        Module MainModule { get; set; }

        Module Load(string request, Module parent, bool isMain = false);

        string Resolve(string request, Module requester);
    }
}
