/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System;
using System.Reflection;
using System.Collections.Generic;

public sealed partial class hmPSDynamicLib
{
    public sealed class DllPathResolver
    {
        public DllPathResolver()
        {
            AppDomain.CurrentDomain.AssemblyResolve += CurrentDomain_AssemblyResolve;
        }

        ~DllPathResolver()
        {
            AppDomain.CurrentDomain.AssemblyResolve -= CurrentDomain_AssemblyResolve;
        }

        private static Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            try
            {
                // PowerShellの参照については、タッチしない
                if (args.Name.Contains("System.Management.Automation"))
                {
                    return null;
                }

                var requestingAssembly = args.RequestingAssembly;
                var requestedAssembly = new AssemblyName(args.Name);

                // ①カレントディレクトリの.dllのファイル名部分だけを指定している場合(.NETの既存ライブラリと同じ書き方。
                // もしくは、C#のusingで読み込めなかった場合もこれに該当する
                String currentmacrodirectory = (String)Hidemaru.TMacro.Var["currentmacrodirectory"];
                var targetfullpath = currentmacrodirectory + @"\" + requestedAssembly.Name + ".dll";
                if (System.IO.File.Exists(targetfullpath))
                {
                    return Assembly.LoadFile(targetfullpath);
                }

                // ②そのようなフルパスが指定されている場合(フルパスを指定した書き方)
                targetfullpath = requestedAssembly.Name;
                if (System.IO.File.Exists(targetfullpath))
                {
                    // そのファイルのディレクトリ
                    var normalizedfullpath = System.IO.Path.GetFullPath(targetfullpath);
                    var targetdirectory = System.IO.Path.GetDirectoryName(normalizedfullpath);

                    return Assembly.LoadFile(targetfullpath);
                }

                // ③パスが特別に登録されている
                List<String> asm_path = (List<String>)runspace.SessionStateProxy.GetVariable("AssemblyPath");

                if (asm_path != null)
                {
                    foreach (var dir in asm_path)
                    {
                        targetfullpath = dir + @"\" + requestedAssembly.Name + ".dll";
                        if (System.IO.File.Exists(targetfullpath))
                        {
                            return Assembly.LoadFile(targetfullpath);
                        }
                    }
                }
            }
            catch (Exception)
            {
            }

            return null;
        }
    }
}
