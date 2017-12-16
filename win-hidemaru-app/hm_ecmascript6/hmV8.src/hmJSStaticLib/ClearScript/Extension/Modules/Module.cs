﻿/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System.Collections.Generic;
using Microsoft.ClearScript;
using Microsoft.ClearScript.V8;
using hmV8DynamicNS.NativeTypes;

namespace hmV8DynamicNS
{
    public class Module
    {
        public Module(string id, IVirtualPath virtualPath, Module parent, IModuleLoader loader)
        {
            Evaluator = loader.Evaluator;
            Id = id;
            Parent = parent;
            Loader = loader;
            VirtualPath = virtualPath;
            parent?.Children?.Add(this);
            Loaded = false;
            Children = new List<Module>();
            RequireFunction = new RequireFunction(Evaluator, loader, this);
        }

        public V8ScriptEngine Evaluator { get; }

        [ScriptMember("id")]
        public string Id { get; }

        [ScriptMember("filename")]
        public string FileName => VirtualPath.ResolvePath();

        public IVirtualPath VirtualPath { get; }

        [ScriptMember("parent")]
        public Module Parent { get; }

        public IModuleLoader Loader { get; }

        public List<Module> Children { get; }

        [ScriptMember("loaded")]
        public bool Loaded { get; set; }

        [ScriptMember("exports")]
        public virtual object Exports { get; set; }

        public RequireFunction RequireFunction { get; }

        [ScriptMember("require")]
        [NativeObject("Function")]
        public object NativeRequire => RequireFunction.GetScriptObject();

        [ScriptMember("children")]
        [NativeObject("Array")]
        public object NativeChildren => new JArray(Evaluator, Children).GetScriptObject();

        public Module Require(string id)
        {
            return Loader.Load(id, this);
        }
    }
}
