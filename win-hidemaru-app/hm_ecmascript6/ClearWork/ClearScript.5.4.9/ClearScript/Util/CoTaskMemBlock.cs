﻿// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
// Microsoft Public License (MS-PL)
// 
// This license governs use of the accompanying software. If you use the
// software, you accept this license. If you do not accept the license, do not
// use the software.
// 
// 1. Definitions
// 
//   The terms "reproduce," "reproduction," "derivative works," and
//   "distribution" have the same meaning here as under U.S. copyright law. A
//   "contribution" is the original software, or any additions or changes to
//   the software. A "contributor" is any person that distributes its
//   contribution under this license. "Licensed patents" are a contributor's
//   patent claims that read directly on its contribution.
// 
// 2. Grant of Rights
// 
//   (A) Copyright Grant- Subject to the terms of this license, including the
//       license conditions and limitations in section 3, each contributor
//       grants you a non-exclusive, worldwide, royalty-free copyright license
//       to reproduce its contribution, prepare derivative works of its
//       contribution, and distribute its contribution or any derivative works
//       that you create.
// 
//   (B) Patent Grant- Subject to the terms of this license, including the
//       license conditions and limitations in section 3, each contributor
//       grants you a non-exclusive, worldwide, royalty-free license under its
//       licensed patents to make, have made, use, sell, offer for sale,
//       import, and/or otherwise dispose of its contribution in the software
//       or derivative works of the contribution in the software.
// 
// 3. Conditions and Limitations
// 
//   (A) No Trademark License- This license does not grant you rights to use
//       any contributors' name, logo, or trademarks.
// 
//   (B) If you bring a patent claim against any contributor over patents that
//       you claim are infringed by the software, your patent license from such
//       contributor to the software ends automatically.
// 
//   (C) If you distribute any portion of the software, you must retain all
//       copyright, patent, trademark, and attribution notices that are present
//       in the software.
// 
//   (D) If you distribute any portion of the software in source code form, you
//       may do so only under this license by including a complete copy of this
//       license with your distribution. If you distribute any portion of the
//       software in compiled or object code form, you may only do so under a
//       license that complies with this license.
// 
//   (E) The software is licensed "as-is." You bear the risk of using it. The
//       contributors give no express warranties, guarantees or conditions. You
//       may have additional consumer rights under your local laws which this
//       license cannot change. To the extent permitted under your local laws,
//       the contributors exclude the implied warranties of merchantability,
//       fitness for a particular purpose and non-infringement.
//       

using System;
using System.Runtime.InteropServices;

namespace Microsoft.ClearScript.Util
{
    internal class CoTaskMemBlock : IDisposable
    {
        public IntPtr Addr { get; private set; }

        public CoTaskMemBlock(int size)
        {
            Addr = Marshal.AllocCoTaskMem(size);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (Addr != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(Addr);
                Addr = IntPtr.Zero;
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~CoTaskMemBlock()
        {
            Dispose(false);
        }
    }

    internal class CoTaskMemArrayBlock : CoTaskMemBlock
    {
        private readonly int elementSize;
        private readonly int length;

        public CoTaskMemArrayBlock(int elementSize, int length)
            : base(elementSize * length)
        {
            this.elementSize = elementSize;
            this.length = length;
        }

        public IntPtr GetAddr(int index)
        {
            if ((index < 0) || (index >= length))
            {
                throw new ArgumentOutOfRangeException("index");
            }

            if (Addr == IntPtr.Zero)
            {
                throw new ObjectDisposedException(ToString());
            }

            return GetAddrInternal(index);
        }

        private IntPtr GetAddrInternal(int index)
        {
            return Addr + (index * elementSize);
        }
    }

    internal class CoTaskMemVariantBlock : CoTaskMemBlock
    {
        public CoTaskMemVariantBlock()
            : base(RawCOMHelpers.VariantSize)
        {
            NativeMethods.VariantInit(Addr);
        }

        public CoTaskMemVariantBlock(object obj)
            : base(RawCOMHelpers.VariantSize)
        {
            Marshal.GetNativeVariantForObject(obj, Addr);
        }

        #region CoTaskMemBlock overrides

        protected override void Dispose(bool disposing)
        {
            if (Addr != IntPtr.Zero)
            {
                NativeMethods.VariantClear(Addr);
            }

            base.Dispose(disposing);
        }

        #endregion
    }

    internal class CoTaskMemVariantArgsBlock : CoTaskMemBlock
    {
        private readonly int length;

        public CoTaskMemVariantArgsBlock(object[] args)
            : base(args.Length * RawCOMHelpers.VariantSize)
        {
            length = args.Length;
            for (var index = 0; index < length; index++)
            {
                Marshal.GetNativeVariantForObject(args[index], GetAddrInternal(length - 1 - index));
            }
        }

        public IntPtr GetAddr(int index)
        {
            if ((index < 0) || (index >= length))
            {
                throw new ArgumentOutOfRangeException("index");
            }

            if (Addr == IntPtr.Zero)
            {
                throw new ObjectDisposedException(ToString());
            }

            return GetAddrInternal(length - 1 - index);
        }

        private IntPtr GetAddrInternal(int index)
        {
            return Addr + (index * RawCOMHelpers.VariantSize);
        }

        #region CoTaskMemBlock overrides

        protected override void Dispose(bool disposing)
        {
            if (Addr != IntPtr.Zero)
            {
                for (var index = 0; index < length; index++)
                {
                    NativeMethods.VariantClear(GetAddrInternal(index));
                }
            }

            base.Dispose(disposing);
        }

        #endregion
    }

    internal class CoTaskMemVariantArgsByRefBlock : CoTaskMemBlock
    {
        private readonly object[] args;

        public CoTaskMemVariantArgsByRefBlock(object[] args)
            : base(args.Length * 2 * RawCOMHelpers.VariantSize)
        {
            this.args = args;
            for (var index = 0; index < args.Length; index++)
            {
                var pArg = GetAddrInternal(args.Length + index);
                Marshal.GetNativeVariantForObject(args[index], pArg);

                var pArgRef = GetAddrInternal(args.Length - 1 - index);
                NativeMethods.VariantInit(pArgRef);
                Marshal.WriteInt16(pArgRef, 0, 0x400C /*VT_BYREF|VT_VARIANT*/);
                Marshal.WriteIntPtr(pArgRef, sizeof(ushort) * 4, pArg);
            }
        }
        public IntPtr GetAddr(int index)
        {
            if ((index < 0) || (index >= args.Length))
            {
                throw new ArgumentOutOfRangeException("index");
            }

            if (Addr == IntPtr.Zero)
            {
                throw new ObjectDisposedException(ToString());
            }

            return GetAddrInternal(args.Length - 1 - index);
        }

        private IntPtr GetAddrInternal(int index)
        {
            return Addr + (index * RawCOMHelpers.VariantSize);
        }

        #region CoTaskMemBlock overrides

        protected override void Dispose(bool disposing)
        {
            if (Addr != IntPtr.Zero)
            {
                for (var index = 0; index < args.Length; index++)
                {
                    var pArg = GetAddrInternal(args.Length + index);
                    args[index] = Marshal.GetObjectForNativeVariant(pArg);
                    NativeMethods.VariantClear(pArg);
                }
            }

            base.Dispose(disposing);
        }

        #endregion
    }
}
