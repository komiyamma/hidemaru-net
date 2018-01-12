// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.ClearScript.Util
{
    internal static class MiscHelpers
    {
        #region COM helpers

        public static object CreateCOMObject(string progID, string serverName)
        {
            return Activator.CreateInstance(GetCOMType(progID, serverName));
        }

        public static object CreateCOMObject(Guid clsid, string serverName)
        {
            return Activator.CreateInstance(GetCOMType(clsid, serverName));
        }

        public static bool TryCreateCOMObject<T>(string progID, string serverName, out T obj) where T : class
        {
            Type type;
            if (!TryGetCOMType(progID, serverName, out type))
            {
                obj = null;
                return false;
            }

            obj = Activator.CreateInstance(type) as T;
            return obj != null;
        }

        public static bool TryCreateCOMObject<T>(Guid clsid, string serverName, out T obj) where T : class
        {
            Type type;
            if (!TryGetCOMType(clsid, serverName, out type))
            {
                obj = null;
                return false;
            }

            obj = Activator.CreateInstance(type) as T;
            return obj != null;
        }

        public static Type GetCOMType(string progID, string serverName)
        {
            VerifyNonBlankArgument(progID, "progID", "Invalid programmatic identifier (ProgID)");

            Type type;
            if (!TryGetCOMType(progID, serverName, out type))
            {
                throw new TypeLoadException(FormatInvariant("Could not find a registered class for '{0}'", progID));
            }

            return type;
        }

        public static Type GetCOMType(Guid clsid, string serverName)
        {
            Type type;
            if (!TryGetCOMType(clsid, serverName, out type))
            {
                throw new TypeLoadException(FormatInvariant("Could not find a registered class for '{0}'", clsid.ToString("B")));
            }

            return type;
        }

        public static bool TryGetCOMType(string progID, string serverName, out Type type)
        {
            Guid clsid;
            type = Guid.TryParseExact(progID, "B", out clsid) ? Type.GetTypeFromCLSID(clsid, serverName) : Type.GetTypeFromProgID(progID, serverName);
            return type != null;
        }

        public static bool TryGetCOMType(Guid clsid, string serverName, out Type type)
        {
            type = Type.GetTypeFromCLSID(clsid, serverName);
            return type != null;
        }

        public static string GetDispIDName(int dispid)
        {
            return FormatInvariant("[DISPID={0}]", dispid);
        }

        #endregion

        #region argument helpers

        public static void VerifyNonNullArgument(object value, string name)
        {
            if (value == null)
            {
                throw new ArgumentNullException(name);
            }
        }

        public static void VerifyNonBlankArgument(string value, string name, string message)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                throw new ArgumentException(message, name);
            }
        }

        #endregion

        #region string helpers

        public static string EnsureNonBlank(string input, string alternate)
        {
            Debug.Assert(!string.IsNullOrWhiteSpace(alternate));
            return string.IsNullOrWhiteSpace(input) ? alternate : input;
        }

        public static string FormatInvariant(string format, params object[] args)
        {
            return string.Format(CultureInfo.InvariantCulture, format, args);
        }

        public static StringBuilder AppendInvariant(this StringBuilder builder, string format, params object[] args)
        {
            return builder.AppendFormat(CultureInfo.InvariantCulture, format, args);
        }

        public static string FormatCode(string code)
        {
            var lines = (code ?? string.Empty).Replace("\r\n", "\n").Split('\n');

            lines = lines.SkipWhile(string.IsNullOrWhiteSpace).Reverse().SkipWhile(string.IsNullOrWhiteSpace).Reverse().ToArray();
            if (lines.Length > 0)
            {
                var firstLine = lines[0];
                for (var indentLength = firstLine.TakeWhile(char.IsWhiteSpace).Count(); indentLength > 0; indentLength--)
                {
                    var indent = firstLine.Substring(0, indentLength);
                    if (lines.Skip(1).All(line => string.IsNullOrWhiteSpace(line) || line.StartsWith(indent, StringComparison.Ordinal)))
                    {
                        lines = lines.Select(line => string.IsNullOrWhiteSpace(line) ? string.Empty : line.Substring(indent.Length)).ToArray();
                        break;
                    }
                }
            }

            return string.Join("\n", lines) + '\n';
        }

        #endregion

        #region index helpers

        public static bool TryGetIndex(object arg, out int index)
        {
            if (arg != null)
            {
                switch (Type.GetTypeCode(arg.GetType()))
                {
                    case TypeCode.Byte:
                    case TypeCode.SByte:
                    case TypeCode.Int16:
                    case TypeCode.UInt16:
                    case TypeCode.Int32:
                        index = Convert.ToInt32(arg);
                        return true;
                }
            }

            index = -1;
            return false;
        }

        public static bool TryGetIndex(object arg, out long index)
        {
            if (arg != null)
            {
                switch (Type.GetTypeCode(arg.GetType()))
                {
                    case TypeCode.Byte:
                    case TypeCode.SByte:
                    case TypeCode.Int16:
                    case TypeCode.UInt16:
                    case TypeCode.Int32:
                    case TypeCode.UInt32:
                    case TypeCode.Int64:
                        index = Convert.ToInt64(arg);
                        return true;
                }
            }

            index = -1;
            return false;
        }

        #endregion

        #region simplified exception handling

        public static bool Try(Action action)
        {
            try
            {
                action();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        public static bool Try<T>(out T result, Func<T> func)
        {
            try
            {
                result = func();
                return true;
            }
            catch (Exception)
            {
                result = default(T);
                return false;
            }
        }

        #endregion

        #region primitive marshaling

        public static bool TryMarshalPrimitiveToHost(object obj, out object result)
        {
            var convertible = obj as IConvertible;
            if (convertible != null)
            {
                switch (convertible.GetTypeCode())
                {
                    case TypeCode.String:
                    case TypeCode.Boolean:
                        result = obj;
                        return true;

                    case TypeCode.Double:
                    case TypeCode.Single:
                        result = MarshalDoubleToHost(convertible.ToDouble(CultureInfo.InvariantCulture));
                        return true;

                    case TypeCode.SByte:
                    case TypeCode.Byte:
                    case TypeCode.Int16:
                    case TypeCode.UInt16:
                    case TypeCode.Char:
                    case TypeCode.Int32:
                    case TypeCode.UInt32:
                    case TypeCode.Int64:
                    case TypeCode.UInt64:
                    case TypeCode.Decimal:
                        result = obj;
                        return true;
                }
            }

            result = null;
            return false;
        }

        public static object MarshalDoubleToHost(double value)
        {
            // ReSharper disable CompareOfFloatsByEqualityOperator

            if (Math.Round(value) == value)
            {
                const double maxIntInDouble = (1L << 53) - 1;
                if (Math.Abs(value) <= maxIntInDouble)
                {
                    var longValue = Convert.ToInt64(value);
                    if ((longValue >= int.MinValue) && (longValue <= int.MaxValue))
                    {
                        return (int)longValue;
                    }

                    return longValue;
                }
            }
            else
            {
                var floatValue = Convert.ToSingle(value);
                if (value == floatValue)
                {
                    return floatValue;
                }
            }

            return value;

            // ReSharper restore CompareOfFloatsByEqualityOperator
        }

        #endregion

        #region miscellaneous

        public static T Exchange<T>(ref T target, T value)
        {
            var oldValue = target;
            target = value;
            return oldValue;
        }

        public static bool IsX86InstructionSet()
        {
            SystemInfo info;
            try
            {
                NativeMethods.GetNativeSystemInfo(out info);
            }
            catch (EntryPointNotFoundException)
            {
                NativeMethods.GetSystemInfo(out info);
            }

            return
                ((info.ProcessorArchitecture == 0 /*PROCESSOR_ARCHITECTURE_INTEL*/) ||
                 (info.ProcessorArchitecture == 9 /*PROCESSOR_ARCHITECTURE_AMD64*/));
        }

        public static void QueueNativeCallback(INativeCallback callback)
        {
            ThreadPool.QueueUserWorkItem(state =>
            {
                using (callback)
                {
                    Try(callback.Invoke);
                }
            });
        }

        public static Random CreateSeededRandom()
        {
            return new Random(Convert.ToUInt32(DateTime.Now.Ticks.ToUnsigned() & 0x00000000FFFFFFFFUL).ToSigned());
        }

        public static async Task<IDisposable> CreateLockScopeAsync(this SemaphoreSlim semaphore)
        {
            await semaphore.WaitAsync().ConfigureAwait(false);
            return Scope.Create(null, () => semaphore.Release());
        }

        #endregion
    }
}
