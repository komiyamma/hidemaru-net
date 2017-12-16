﻿/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace hmV8DynamicNS
{
    public class ModulePathResolver : IModulePathResolver
    {
        public ModulePathResolver(string modulesPath, IEnumerable<string> extensions, string defaultFileName)
        {
            // ModulesPath = modulesPath;
            Extensions = extensions;
            DefaultFileName = defaultFileName;
        }

        // ★2017/09/27 書きかえ。秀丸では常にマクロディレクトリがモジュールのパスである。
        public string ModulesPath {
            get { return (string)hmV8DynamicLib.Hidemaru.Macro.__Var("currentmacrodirectory"); }
        }

        public IEnumerable<string> Extensions { get; set; }

        public string DefaultFileName { get; set; }

        public IVirtualPath Resolve(string path, Module parent)
        {
            if (string.IsNullOrEmpty(path))
            {
                throw new ArgumentException("Invalid file path.");
            }

            string combinedPath;
            if (path.StartsWith("/"))
            {
                combinedPath = path.Substring(1);
            }
            else if (path.Length > 1)
            {
                if (path.StartsWith("./") || path.StartsWith("../"))
                {
                    var parentDirectory = Path.GetDirectoryName(parent.FileName);
                    if (parentDirectory == null)
                    {
                        return null;
                    }

                    combinedPath = Path.Combine(parentDirectory, path);
                }
                else
                {
                    combinedPath = Path.Combine(ModulesPath, path);
                }
            }
            else
            {
                combinedPath = Path.Combine(ModulesPath, path);
            }

            var result = SearchFile(combinedPath) ?? SearchDirectory(combinedPath);
            if (result == null)
            {
                return null;
            }

            return new FilePath(result);
        }

        private string SearchFile(string file)
        {
            if (File.Exists(file))
            {
                return Path.GetFullPath(file);
            }

            // ReSharper disable once LoopCanBeConvertedToQuery
            foreach (var extension in Extensions)
            {
                var filePath = file + extension;
                if (File.Exists(filePath))
                {
                    return Path.GetFullPath(filePath);
                }
            }

            return null;
        }

        private string SearchDirectory(string directory)
        {
            if (!Directory.Exists(directory)) return null;
            return (from extension in Extensions
                    let file = Path.Combine(directory, DefaultFileName) + extension
                    where File.Exists(file)
                    select Path.GetFullPath(file))
                    .FirstOrDefault();
        }
    }
}
