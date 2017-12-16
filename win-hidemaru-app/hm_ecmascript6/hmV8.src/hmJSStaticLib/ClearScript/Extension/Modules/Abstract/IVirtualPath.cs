/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

namespace hmV8DynamicNS
{
    public interface IVirtualPath
    {
        string Identifier { get; }

        string ResolvePath();

        string ResolveDirectory();

        string ResolveExtension();

        string ResolveContent();
    }
}
