// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using Microsoft.ClearScript.Util.COM;
using TYPEFLAGS = System.Runtime.InteropServices.ComTypes.TYPEFLAGS;
using TYPEKIND = System.Runtime.InteropServices.ComTypes.TYPEKIND;

namespace Microsoft.ClearScript.Util
{
    internal static partial class ObjectHelpers
    {
        public static Type GetTypeOrTypeInfo(this object value)
        {
            if (!MiscHelpers.PlatformIsWindows())
            {
                return value.GetType();
            }

            var type = value.GetType();
            IDispatch dispatch = null;

            Type typeInfo = null;
            TYPEKIND typeInfoKind = 0;
            TYPEFLAGS typeInfoFlags = 0;

            if (type.IsUnknownCOMObject())
            {
                // This appears to be a generic COM object with no specific type information.
                // Attempt to acquire COM type information via IDispatch or IProvideClassInfo.

                dispatch = value as IDispatch;
                if (dispatch != null)
                {
                    var tempTypeInfo = dispatch.GetTypeInfo();
                    if (tempTypeInfo != null)
                    {
                        typeInfo = GetTypeForTypeInfo(tempTypeInfo);
                        typeInfoKind = tempTypeInfo.GetKind();
                        typeInfoFlags = tempTypeInfo.GetFlags();
                    }
                }

                if (typeInfo == null)
                {
                    if (value is IProvideClassInfo provideClassInfo)
                    {
                        if (HResult.Succeeded(provideClassInfo.GetClassInfo(out var tempTypeInfo)))
                        {
                            typeInfo = GetTypeForTypeInfo(tempTypeInfo);
                            typeInfoKind = tempTypeInfo.GetKind();
                            typeInfoFlags = tempTypeInfo.GetFlags();
                        }
                    }
                }
            }

            if (typeInfo != null)
            {
                // If the COM type is a dispatch-only interface, use it. Such interfaces typically
                // aren't exposed via QueryInterface(), so there's no way to validate them anyway.

                if ((dispatch != null) && (typeInfoKind == TYPEKIND.TKIND_DISPATCH) && typeInfoFlags.HasFlag(TYPEFLAGS.TYPEFLAG_FDISPATCHABLE) && !typeInfoFlags.HasFlag(TYPEFLAGS.TYPEFLAG_FDUAL))
                {
                    return typeInfo;
                }

                // COM type information acquired in this manner may not actually be valid for the
                // original object. In some cases the original object implements a base interface.

                if (typeInfo.IsInstanceOfType(value))
                {
                    return typeInfo;
                }

                foreach (var interfaceType in typeInfo.GetInterfaces())
                {
                    if (interfaceType.IsInstanceOfType(value))
                    {
                        return interfaceType;
                    }
                }
            }

            return type;
        }

        private static Type GetTypeForTypeInfo(ITypeInfo typeInfo)
        {
            // ReSharper disable EmptyGeneralCatchClause

            try
            {
                var typeLib = typeInfo.GetContainingTypeLib(out var index);

                var assembly = LoadPrimaryInteropAssembly(typeLib);
                if (assembly != null)
                {
                    var name = typeInfo.GetManagedName();
                    var guid = typeInfo.GetGuid();

                    var type = assembly.GetType(name, false, true);
                    if ((type != null) && (type.GUID == guid))
                    {
                        return type;
                    }

                    var types = assembly.GetAllTypes().ToArray();
                    if ((index >= 0) && (index < types.Length))
                    {
                        type = types[index];
                        if ((type.GUID == guid) && (type.FullName == name))
                        {
                            return type;
                        }
                    }

                    type = types.FirstOrDefault(testType => (testType.GUID == guid) && (testType.FullName.Equals(name, StringComparison.OrdinalIgnoreCase)));
                    if (type != null)
                    {
                        return type;
                    }
                }

                return typeInfo.GetManagedType();
            }
            catch (Exception)
            {
            }

            return null;

            // ReSharper restore EmptyGeneralCatchClause
        }

        private static Assembly LoadPrimaryInteropAssembly(ITypeLib typeLib)
        {
            if (typeLib == null)
            {
                return null;
            }

            // ReSharper disable EmptyGeneralCatchClause

            try
            {
                using (var attrScope = typeLib.CreateAttrScope())
                {
                    if (GetPrimaryInteropAssembly(attrScope.Value.guid, attrScope.Value.wMajorVerNum, attrScope.Value.wMinorVerNum, out var name, out var codeBase))
                    {
                        return Assembly.Load(new AssemblyName(name) { CodeBase = codeBase });
                    }
                }
            }
            catch (Exception)
            {
            }

            return null;

            // ReSharper restore EmptyGeneralCatchClause
        }

        public static string GetFriendlyName(this object value)
        {
            return value.GetFriendlyName(null);
        }

        public static string GetFriendlyName(this object value, Type type)
        {
            if (type == null)
            {
                if (value == null)
                {
                    return "[null]";
                }

                type = value.GetType();
            }

            if (type.IsArray && (value != null))
            {
                var array = (Array)value;
                var dimensions = Enumerable.Range(0, type.GetArrayRank());
                var lengths = string.Join(",", dimensions.Select(array.GetLength));
                return MiscHelpers.FormatInvariant("{0}[{1}]", type.GetElementType().GetFriendlyName(), lengths);
            }

            if (type.IsUnknownCOMObject())
            {
                if (value is IDispatch dispatch)
                {
                    var typeInfo = dispatch.GetTypeInfo();
                    if (typeInfo != null)
                    {
                        return typeInfo.GetName();
                    }
                }
            }

            return type.GetFriendlyName();
        }

        public static T DynamicCast<T>(this object value)
        {
            // ReSharper disable RedundantCast

            // the cast to dynamic is not redundant; removing it breaks tests
            return (T)(dynamic)value;

            // ReSharper restore RedundantCast
        }

        public static object ToDynamicResult(this object result, ScriptEngine engine)
        {
            if (result is Nonexistent)
            {
                return Undefined.Value;
            }

            if ((result is HostTarget) || (result is IPropertyBag))
            {
                // Returning an instance of HostTarget (an internal type) isn't likely to be
                // useful. Wrapping it in a dynamic object makes sense in this context. Wrapping
                // a property bag allows it to participate in dynamic invocation chaining, which
                // may be useful when dealing with things like host type collections. HostItem
                // supports dynamic conversion, so the client can unwrap the object if necessary.

                return HostItem.Wrap(engine, result);
            }

            return result;
        }

        #region Nested type: IProvideClassInfo

        [ComImport]
        [Guid("b196b283-bab4-101a-b69c-00aa00341d07")]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        private interface IProvideClassInfo
        {
            [PreserveSig]
            int GetClassInfo(
                [Out] [MarshalAs(UnmanagedType.Interface)] out ITypeInfo typeInfo
            );
        }

        #endregion
    }
}
