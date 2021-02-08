using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

internal class DllAssemblyResolver
{
    public DllAssemblyResolver()
    {
        AppDomain.CurrentDomain.AssemblyResolve += CurrentDomain_AssemblyResolve;
    }

    ~DllAssemblyResolver()
    {
        AppDomain.CurrentDomain.AssemblyResolve -= CurrentDomain_AssemblyResolve;
    }

    private Assembly CurrentDomain_AssemblyResolve(object sender, ResolveEventArgs args)
    {
        try
        {
            var requestingAssembly = args.RequestingAssembly;
            var requestedAssembly = new AssemblyName(args.Name);
            System.Diagnostics.Trace.WriteLine($"CurrentDomain_AssemblyResolve:{args.Name}");

            // このdll自体を置いているフォルダに読み込み対象のアセンブリがあるかもしれない。
            String self_full_path = Assembly.GetExecutingAssembly().Location;
            String self_dir = Path.GetDirectoryName(self_full_path);

            var targetfullpath = self_dir + $@"\{requestedAssembly.Name}.dll";

            if (File.Exists(targetfullpath))
            {
                return Assembly.LoadFile(targetfullpath);
            }

            // そのようなフルパスが指定されている場合(フルパスを指定した書き方)
            targetfullpath = requestedAssembly.Name;
            if (File.Exists(targetfullpath))
            {
                return Assembly.LoadFile(targetfullpath);
            }
        }
        catch (Exception ex)
        {
            return null;
        }
        return null;
    }
}


