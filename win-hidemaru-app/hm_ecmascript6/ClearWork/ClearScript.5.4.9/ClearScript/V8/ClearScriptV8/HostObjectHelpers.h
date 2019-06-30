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

#pragma once

//-----------------------------------------------------------------------------
// HostObjectHelpers
//-----------------------------------------------------------------------------

class HostObjectHelpers
{
    PROHIBIT_CONSTRUCT(HostObjectHelpers)

public:

    static void* AddRef(void* pvObject);
    static void Release(void* pvObject);

    static V8Value GetProperty(void* pvObject, const StdString& name);
    static V8Value GetProperty(void* pvObject, const StdString& name, bool& isCacheable);
    static void SetProperty(void* pvObject, const StdString& name, const V8Value& value);
    static bool DeleteProperty(void* pvObject, const StdString& name);
    static void GetPropertyNames(void* pvObject, std::vector<StdString>& names);

    static V8Value GetProperty(void* pvObject, int index);
    static void SetProperty(void* pvObject, int index, const V8Value& value);
    static bool DeleteProperty(void* pvObject, int index);
    static void GetPropertyIndices(void* pvObject, std::vector<int>& indices);

    static V8Value Invoke(void* pvObject, const std::vector<V8Value>& args, bool asConstructor);
    static V8Value InvokeMethod(void* pvObject, const StdString& name, const std::vector<V8Value>& args);
    static bool IsDelegate(void* pvObject);

    static V8Value GetEnumerator(void* pvObject);
    static bool AdvanceEnumerator(void* pvEnumerator, V8Value& value);

    static void* CreateV8ObjectCache();
    static void CacheV8Object(void* pvCache, void* pvObject, void* pvV8Object);
    static void* GetCachedV8Object(void* pvCache, void* pvObject);
    static void GetAllCachedV8Objects(void* pvCache, std::vector<void*>& v8ObjectPtrs);
    static bool RemoveV8ObjectCacheEntry(void* pvCache, void* pvObject);

    enum class DebugDirective { SendDebugCommand, DispatchDebugMessages };
    typedef std::function<void(DebugDirective directive, const StdString* pCommand)> DebugCallback;
    static void* CreateDebugAgent(const StdString& name, const StdString& version, int port, DebugCallback&& callback);
    static void SendDebugMessage(void* pvAgent, const StdString& content);
    static void DestroyDebugAgent(void* pvAgent);

    typedef std::function<void()> NativeCallback;
    static void QueueNativeCallback(NativeCallback&& callback);
    static void* CreateNativeCallbackTimer(int dueTime, int period, NativeCallback&& callback);
    static bool ChangeNativeCallbackTimer(void* pvTimer, int dueTime, int period);
    static void DestroyNativeCallbackTimer(void* pvTimer);

    static bool TryParseInt32(const StdString& text, int& result);
};
