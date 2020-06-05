/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;
using System.Collections.Generic;
using Microsoft.ClearScript;
using Microsoft.ClearScript.V8;
using hmV8DynamicNS.Compilers;
using hmV8DynamicNS.Extensions;

// [assembly: NoDefaultScriptAccess]

namespace hmV8DynamicNS
{
    public partial class V8EngineCore : IDisposable
    {
        private readonly V8ScriptEngine engine;
        private readonly IModuleLoader loader;
        private readonly IModulePathResolver pathResolver;
        private readonly NativeModule coreModule;

        public V8EngineCore(IModulePathResolver pathResolver, IModuleLoaderFactory loaderFactory, int iDebuggingPort)
        {
            // デバッグありモード
            if (iDebuggingPort > 0)
            {
                engine = new V8ScriptEngine(V8ScriptEngineFlags.EnableDebugging|V8ScriptEngineFlags.EnableDynamicModuleImports, iDebuggingPort);
            }
            else
            {
                engine = new V8ScriptEngine(V8ScriptEngineFlags.EnableDynamicModuleImports);
            }

            engine.DefaultAccess = ScriptAccess.Full;
            engine.SuppressExtensionMethodEnumeration = true;
            engine.AllowReflection = true;

            Engine = engine;
            this.pathResolver = pathResolver;
            NativeModules = new Dictionary<string, Module>();
            Compilers = new Dictionary<string, IModuleCompiler>();
            loader = loaderFactory.Create(Engine, NativeModules, Compilers, pathResolver);

            var scriptCompiler = new ScriptCompiler();
            Compilers[".js"] = scriptCompiler;
            Compilers[".json"] = new JsonCompiler();
            Compilers[".dll"] = new DllCompiler();
            coreModule = new NativeModule("core", loader, this);

            NativeModules["core"] = coreModule;
            //            NativeModules["host"] = new NativeModule("host", loader, new HostFunctions());
            //            NativeModules["xhost"] = new NativeModule("xhost", loader, new ExtendedHostFunctions());
            NativeModules["host"] = new NativeModule("host", loader, new ExtendedHostFunctions());

            engine.AddHostObject("clr", new HostTypeCollection("mscorlib", "System", "System.Core"));
            engine.AddHostObject("host", new ExtendedHostFunctions());

            foreach (var script in ScriptModules.Scripts.Keys)
            {
                NativeModules.Add(script, new ScriptModule(script, loader));
            }

            engine.Script.core = this;
            engine.Script.EngineInternal.isVoid = new Func<object, bool>(obj => obj is VoidResult);
            ExecuteWrapped(@"
                Object.defineProperty(this, 'global', { value: this, enumerable: true });
                var engineInternal = this.EngineInternal;
                delete this.EngineInternal;
                Object.defineProperty(this, 'EngineInternal', { value: engineInternal });
            ");

            Sleeping = true;
        }

        public Dictionary<string, Module> NativeModules { get; }

        public Dictionary<string, IModuleCompiler> Compilers { get; }

        public V8ScriptEngine Engine { get; }

        public Module Run(string request)
        {
            if (!Sleeping)
            {
                throw new InvalidOperationException("Running at this state is not permitted.");
            }

            Sleeping = false;
            try
            {
                return loader.Load(request, null, true);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Trace.WriteLine(ex.GetScriptStack());
                Sleeping = true;
                throw;
            }
        }

        #region Native Modules

        #endregion

        [ScriptMember("sleeping")]
        public bool Sleeping { get; private set; }

        [ScriptMember("all")]
        public bool FullAccess
        {
            get { return engine.DefaultAccess != ScriptAccess.None; }
            set { engine.DefaultAccess = value ? ScriptAccess.Full : ScriptAccess.None; }
        }

        [ScriptMember("disposed")]
        public bool Disposed { get; private set; }


        [NativeObjectHint]
        public object Require(string request)
        {
            return coreModule.RequireFunction.Invoke(request);
        }

        [ScriptMember("addNativeModule")]
        public void AddNativeModule(string name, object exports)
        {
            if (NativeModules.ContainsKey(name))
            {
                throw new InvalidOperationException("A native module with the same name already exists.");
            }

            NativeModules[name] = new NativeModule(name, loader, exports);
        }

        [ScriptMember("exposeRequire")]
        public object ExposeGlobalRequire()
        {
            return engine.Script.require = coreModule.RequireFunction.GetScriptObject();
        }

        public void Execute(string code) => ExecuteWrapped(code);

        [ScriptMember("executeCommand")]
        public string ExecuteCommand(string command) => Engine.ExecuteCommand(command);

        private void ExecuteWrapped(string code)
        {
            engine.Execute($@"
                (function () {{
                    {code}
                }})()");
        }

        public void Dispose()
        {
            if (Disposed)
            {
                return;
            }

            Disposed = true;
        }
    }
}
