/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

using System.Reflection;
using Microsoft.ClearScript.Util;



// ファイルをフルパスで指定して、アセンブリをロードできるようにする
namespace Microsoft.ClearScript
{
    public partial class HostTypeCollection : PropertyBag
    {
        /// <summary>
        /// Adds types from an assembly to a host type collection. The assembly is specified by name.
        /// </summary>
        /// <param name="assemblyName">The name of the assembly that contains the types to add.</param>
        public void AddAssembly(string assemblyName)
        {
            // そもそもターゲットとなっている場所に存在する(多分フルパスで指定している、など)
            if (System.IO.File.Exists(assemblyName))
            {
                AddAssembly(Assembly.LoadFile(assemblyName));
            }
            else
            {
                MiscHelpers.VerifyNonBlankArgument(assemblyName, "assemblyName", "Invalid assembly name");
                AddAssembly(Assembly.Load(AssemblyTable.GetFullAssemblyName(assemblyName)));
            }
        }
    }
}

