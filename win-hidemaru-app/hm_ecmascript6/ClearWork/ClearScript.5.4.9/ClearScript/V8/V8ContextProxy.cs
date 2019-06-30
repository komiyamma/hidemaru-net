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

namespace Microsoft.ClearScript.V8
{
    internal abstract class V8ContextProxy : V8Proxy
    {
        public static V8ContextProxy Create(V8IsolateProxy isolateProxy, string name, bool enableDebugging, bool disableGlobalMembers, int debugPort)
        {
            return CreateImpl<V8ContextProxy>(isolateProxy, name, enableDebugging, disableGlobalMembers, debugPort);
        }

        public abstract UIntPtr MaxRuntimeHeapSize { get; set; }

        public abstract TimeSpan RuntimeHeapSizeSampleInterval { get; set; }

        public abstract UIntPtr MaxRuntimeStackUsage { get; set; }

        public abstract void InvokeWithLock(Action action);

        public abstract object GetRootItem();

        public abstract void AddGlobalItem(string name, object item, bool globalMembers);

        public abstract object Execute(string documentName, string code, bool evaluate, bool discard);

        public abstract V8Script Compile(string documentName, string code);

        public abstract V8Script Compile(string documentName, string code, V8CacheKind cacheKind, out byte[] cacheBytes);

        public abstract V8Script Compile(string documentName, string code, V8CacheKind cacheKind, byte[] cacheBytes, out bool cacheAccepted);

        public abstract object Execute(V8Script script, bool evaluate);

        public abstract void Interrupt();

        public abstract V8RuntimeHeapInfo GetRuntimeHeapInfo();

        public abstract void CollectGarbage(bool exhaustive);

        public abstract void OnAccessSettingsChanged();
    }
}
