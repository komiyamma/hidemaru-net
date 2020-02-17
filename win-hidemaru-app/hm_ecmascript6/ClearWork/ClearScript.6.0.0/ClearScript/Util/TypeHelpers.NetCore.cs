// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Reflection;
using System.Runtime.InteropServices.ComTypes;

namespace Microsoft.ClearScript.Util
{
    internal static partial class TypeHelpers
    {
        private static string GetFullTypeName(string name, string assemblyName, bool useAssemblyName, int typeArgCount)
        {
            var fullTypeName = name;

            if (typeArgCount > 0)
            {
                fullTypeName += MiscHelpers.FormatInvariant("`{0}", typeArgCount);
            }

            if (useAssemblyName)
            {
                Assembly assembly;
                if (MiscHelpers.Try(out assembly, () => Assembly.Load(AssemblyTable.GetFullAssemblyName(assemblyName))))
                {
                    // ReSharper disable AccessToModifiedClosure

                    string result;
                    if (MiscHelpers.Try(out result, () => assembly.GetType(fullTypeName).AssemblyQualifiedName))
                    {
                        return result;
                    }

                    // ReSharper restore AccessToModifiedClosure
                }

                fullTypeName += MiscHelpers.FormatInvariant(", {0}", AssemblyTable.GetFullAssemblyName(assemblyName));
            }

            return fullTypeName;
        }

        public static IntPtr GetITypeInfo(this Type type)
        {
            return IntPtr.Zero;
        }

        public static Type GetManagedType(this ITypeInfo typeInfo)
        {
            return null;
        }
    }
}
