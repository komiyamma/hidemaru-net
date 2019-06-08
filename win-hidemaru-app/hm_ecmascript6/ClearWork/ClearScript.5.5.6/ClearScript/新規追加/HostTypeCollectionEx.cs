// Copyright (c) Microsoft Corporation.
// Copyright (c) 2016-2017 Akitsugu Komiyama.

using System;
using System.Collections.Generic;
using System.Linq;
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
        /// 
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


// VBScriptEngine.cs は カット
// JScriptEngine.cs は カット

/*
 *    
 *      protected override object[] AdjustInvokeArgs(object[] args)
        {
            // WORKAROUND: JScript seems to require at least one argument to invoke a function
            return ((engine is JScriptEngine) && (args.Length< 1)) ? new object[] { Undefined.Value } : args;
        }

    は
        protected override object[] AdjustInvokeArgs(object[] args)
        {
            // WORKAROUND: JScript seems to require at least one argument to invoke a function
            return args;
        }
  *
  */



// ビルドイベントはXMLの出力のチェックボックスをOFFにする


// iV8 は 32bit優先のフラグつけておくこと