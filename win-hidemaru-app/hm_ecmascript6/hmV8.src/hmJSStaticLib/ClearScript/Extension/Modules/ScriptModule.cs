/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;

namespace hmV8DynamicNS
{
    public class ScriptModule : Module
    {
        private bool loading;

        public ScriptModule(string id, IModuleLoader loader)
            : base(id, new ScriptModulePath(id), null, loader)
        {
        }

        public override sealed object Exports
        {
            get
            {
                if (loading)
                {
                    throw new InvalidOperationException("Cycles are not allowed in native scripts.");
                }

                if (Loaded)
                {
                    return base.Exports;
                }

                loading = true;
                Loader.Compilers[".js"].Compile(this);
                if (!Loaded)
                {
                    throw new InvalidOperationException("Invalid state reached.");
                }

                loading = false;
                return base.Exports;
            }
            set
            {
                base.Exports = value;
            }
        }
    }
}
