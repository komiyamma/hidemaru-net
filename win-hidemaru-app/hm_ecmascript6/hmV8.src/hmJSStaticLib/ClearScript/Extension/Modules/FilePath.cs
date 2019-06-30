/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System.IO;

namespace hmV8DynamicNS
{
    public class FilePath : IVirtualPath
    {
        public FilePath(string path)
        {
            Identifier = path;
        }

        public string Identifier { get; }

        public string ResolvePath()
        {
            return Identifier;
        }

        public string ResolveDirectory()
        {
            return Path.GetDirectoryName(Identifier);
        }

        public string ResolveExtension()
        {
            return Path.GetExtension(Identifier);
        }

        public string ResolveContent()
        {
            return File.ReadAllText(Identifier);
        }
    }
}
