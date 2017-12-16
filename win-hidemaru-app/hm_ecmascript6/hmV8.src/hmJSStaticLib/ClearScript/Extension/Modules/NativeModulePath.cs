/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;
using hmV8DynamicNS.Helpers;

namespace hmV8DynamicNS
{
    public class NativeModulePath : IVirtualPath
    {
        public NativeModulePath(string identifier)
        {
            Identifier = identifier;
        }

        public string Identifier { get; }

        public string ResolvePath()
        {
            return PathHelpers.GetAssemblyPath();
        }

        public string ResolveDirectory()
        {
            return PathHelpers.GetAssemblyDirectory();
        }

        public string ResolveExtension()
        {
            return "[native]";
        }

        public string ResolveContent()
        {
            throw new NotSupportedException();
        }
    }
}
