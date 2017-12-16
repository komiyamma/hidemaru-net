﻿/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using hmV8DynamicNS.Helpers;

namespace hmV8DynamicNS
{
    public class ScriptModulePath : IVirtualPath
    {
        public ScriptModulePath(string identifier)
        {
            Identifier = identifier;
        }

        public string Identifier { get; }

        public string ResolvePath()
        {
            return "[native]\\" + Identifier + ".js";
        }

        public string ResolveDirectory()
        {
            return PathHelpers.GetAssemblyDirectory();
        }

        public string ResolveExtension()
        {
            return ".js";
        }

        public string ResolveContent()
        {
            return ScriptModules.Scripts[Identifier];
        }
    }
}
