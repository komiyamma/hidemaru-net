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

namespace Microsoft {
namespace ClearScript {
namespace V8 {

    //-------------------------------------------------------------------------
    // V8ContextProxyImpl
    //-------------------------------------------------------------------------

    private ref class V8ContextProxyImpl : V8ContextProxy
    {
    public:

        V8ContextProxyImpl(V8IsolateProxy^ gcIsolateProxy, String^ gcName, Boolean enableDebugging, Boolean disableGlobalMembers, Int32 debugPort);

        property UIntPtr MaxRuntimeHeapSize
        {
            virtual UIntPtr get() override;
            virtual void set(UIntPtr value) override;
        }

        property TimeSpan RuntimeHeapSizeSampleInterval
        {
            virtual TimeSpan get() override;
            virtual void set(TimeSpan value) override;
        }

        property UIntPtr MaxRuntimeStackUsage
        {
            virtual UIntPtr get() override;
            virtual void set(UIntPtr value) override;
        }

        virtual void InvokeWithLock(Action^ gcAction) override;
        virtual Object^ GetRootItem() override;
        virtual void AddGlobalItem(String^ gcName, Object^ gcItem, Boolean globalMembers) override;
        virtual Object^ Execute(String^ gcDocumentName, String^ gcCode, Boolean evaluate, Boolean discard) override;
        virtual V8Script^ Compile(String^ gcDocumentName, String^ gcCode) override;
        virtual V8Script^ Compile(String^ gcDocumentName, String^ gcCode, V8CacheKind cacheKind, [Out] array<Byte>^% gcCacheBytes) override;
        virtual V8Script^ Compile(String^ gcDocumentName, String^ gcCode, V8CacheKind cacheKind, array<Byte>^ gcCacheBytes, [Out] Boolean% cacheAccepted) override;
        virtual Object^ Execute(V8Script^ gcScript, Boolean evaluate) override;
        virtual void Interrupt() override;
        virtual V8RuntimeHeapInfo^ GetRuntimeHeapInfo() override;
        virtual void CollectGarbage(bool exhaustive) override;
        virtual void OnAccessSettingsChanged() override;

        ~V8ContextProxyImpl();
        !V8ContextProxyImpl();

        static V8Value ImportValue(Object^ gcObject);
        static Object^ ExportValue(const V8Value& value);

    private:

        SharedPtr<V8Context> GetContext();

        Object^ m_gcLock;
        SharedPtr<V8Context>* m_pspContext;
    };

}}}
