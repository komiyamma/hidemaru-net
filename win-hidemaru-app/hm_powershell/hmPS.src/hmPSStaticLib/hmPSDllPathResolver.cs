using System;
using System.Reflection;
using System.Collections.Generic;

public partial class hmPSDynamicLib
{
    public class DllPathResolver
    {
        public DllPathResolver()
        {
            dic.Clear();
            AppDomain.CurrentDomain.AssemblyResolve += CurrentDomain_AssemblyResolve;
        }

        ~DllPathResolver()
        {
            AppDomain.CurrentDomain.AssemblyResolve -= CurrentDomain_AssemblyResolve;
            dic.Clear();
        }

        static Dictionary<String, Assembly> dic = new Dictionary<String, Assembly>();

        private static Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
        {
            try
            {
                if (dic.ContainsKey(args.Name))
                {
                    return dic[args.Name];
                }

                var requestingAssembly = args.RequestingAssembly;
                var requestedAssembly = new AssemblyName(args.Name);

                // ①カレントディレクトリの.dllのファイル名部分だけを指定している場合(.NETの既存ライブラリと同じ書き方。
                // もしくは、C#のusingで読み込めなかった場合もこれに該当する
                String currentmacrodirectory = (String)Hidemaru.Var["currentmacrodirectory"];
                var targetfullpath = currentmacrodirectory + @"\" + requestedAssembly.Name + ".dll";
                if (System.IO.File.Exists(targetfullpath))
                {
                    dic[args.Name] = Assembly.LoadFile(targetfullpath);
                    return dic[args.Name];
                }

                // ②そのようなフルパスが指定されている場合(フルパスを指定した書き方)
                targetfullpath = requestedAssembly.Name;
                if (System.IO.File.Exists(targetfullpath))
                {
                    // そのファイルのディレクトリ
                    var normalizedfullpath = System.IO.Path.GetFullPath(targetfullpath);
                    var targetdirectory = System.IO.Path.GetDirectoryName(normalizedfullpath);

                    dic[args.Name] = Assembly.LoadFile(targetfullpath);
                    return dic[args.Name];
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
                            dic[args.Name] = Assembly.LoadFile(targetfullpath);
                            return dic[args.Name];
                        }
                    }
                }
            }
            catch (Exception e)
            {
            }

            dic[args.Name] = null;
            return null;
        }
    }
}
