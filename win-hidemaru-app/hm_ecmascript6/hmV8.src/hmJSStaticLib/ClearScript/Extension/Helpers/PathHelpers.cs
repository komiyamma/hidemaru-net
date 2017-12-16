/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;
using System.IO;
using System.Reflection;

namespace hmV8DynamicNS.Helpers
{
    public static class PathHelpers
    {
        public static string GetAssemblyPath()
        {
            return Path.GetFullPath((new Uri(Assembly.GetExecutingAssembly().CodeBase)).AbsolutePath);
        }

        public static string GetAssemblyDirectory()
        {
            // ReSharper disable once AssignNullToNotNullAttribute
            return
                Path.GetFullPath(Path.GetDirectoryName((new Uri(Assembly.GetExecutingAssembly().CodeBase)).AbsolutePath));
        }
    }
}
