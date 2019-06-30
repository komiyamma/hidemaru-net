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

#include "ClearScriptV8Managed.h"

namespace Microsoft {
namespace ClearScript {
namespace V8 {

    //-------------------------------------------------------------------------
    // V8IsolateProxyImpl implementation
    //-------------------------------------------------------------------------

    V8IsolateProxyImpl::V8IsolateProxyImpl(String^ gcName, V8RuntimeConstraints^ gcConstraints, Boolean enableDebugging, Int32 debugPort):
        m_gcLock(gcnew Object)
    {
        const V8IsolateConstraints* pConstraints = nullptr;

        V8IsolateConstraints constraints;
        if (gcConstraints != nullptr)
        {
            constraints.Set(AdjustConstraint(gcConstraints->MaxNewSpaceSize), AdjustConstraint(gcConstraints->MaxOldSpaceSize), AdjustConstraint(gcConstraints->MaxExecutableSize));
            pConstraints = &constraints;
        }

        try
        {
            m_pspIsolate = new SharedPtr<V8Isolate>(V8Isolate::Create(StdString(gcName), pConstraints, enableDebugging, debugPort));
        }
        catch (const V8Exception& exception)
        {
            exception.ThrowScriptEngineException();
        }
    }

    //-------------------------------------------------------------------------

    UIntPtr V8IsolateProxyImpl::MaxHeapSize::get()
    {
        return (UIntPtr)GetIsolate()->GetMaxHeapSize();
    }

    //-------------------------------------------------------------------------

    void V8IsolateProxyImpl::MaxHeapSize::set(UIntPtr value)
    {
        GetIsolate()->SetMaxHeapSize(static_cast<size_t>(value));
    }

    //-------------------------------------------------------------------------

    TimeSpan V8IsolateProxyImpl::HeapSizeSampleInterval::get()
    {
        return TimeSpan::FromMilliseconds(GetIsolate()->GetHeapSizeSampleInterval());
    }

    //-------------------------------------------------------------------------

    void V8IsolateProxyImpl::HeapSizeSampleInterval::set(TimeSpan value)
    {
        GetIsolate()->SetHeapSizeSampleInterval(value.TotalMilliseconds);
    }

    //-------------------------------------------------------------------------

    UIntPtr V8IsolateProxyImpl::MaxStackUsage::get()
    {
        return (UIntPtr)GetIsolate()->GetMaxStackUsage();
    }

    //-------------------------------------------------------------------------

    void V8IsolateProxyImpl::MaxStackUsage::set(UIntPtr value)
    {
        GetIsolate()->SetMaxStackUsage(static_cast<size_t>(value));
    }

    //-------------------------------------------------------------------------

    V8Script^ V8IsolateProxyImpl::Compile(String^ gcDocumentName, String^ gcCode)
    {
        try
        {
            return gcnew V8ScriptImpl(gcDocumentName, GetIsolate()->Compile(StdString(gcDocumentName), StdString(gcCode)));
        }
        catch (const V8Exception& exception)
        {
            exception.ThrowScriptEngineException();
        }
    }

    //-------------------------------------------------------------------------

    V8Script^ V8IsolateProxyImpl::Compile(String^ gcDocumentName, String^ gcCode, V8CacheKind cacheKind, [Out] array<Byte>^% gcCacheBytes)
    {
        if (cacheKind == V8CacheKind::None)
        {
            gcCacheBytes = nullptr;
            return Compile(gcDocumentName, gcCode);
        }

        try
        {
            std::vector<std::uint8_t> cacheBytes;
            auto cacheType = (cacheKind == V8CacheKind::Parser) ? V8CacheType::Parser : V8CacheType::Code;
            auto gcScript = gcnew V8ScriptImpl(gcDocumentName, GetIsolate()->Compile(StdString(gcDocumentName), StdString(gcCode), cacheType, cacheBytes));

            auto length = static_cast<int>(cacheBytes.size());
            if (length < 1)
            {
                gcCacheBytes = nullptr;
            }
            else
            {
                gcCacheBytes = gcnew array<Byte>(length);
                Marshal::Copy((IntPtr)&cacheBytes[0], gcCacheBytes, 0, length);
            }

            return gcScript;
        }
        catch (const V8Exception& exception)
        {
            exception.ThrowScriptEngineException();
        }
    }

    //-------------------------------------------------------------------------

    V8Script^ V8IsolateProxyImpl::Compile(String^ gcDocumentName, String^ gcCode, V8CacheKind cacheKind, array<Byte>^ gcCacheBytes, [Out] Boolean% cacheAccepted)
    {
        if ((cacheKind == V8CacheKind::None) || (gcCacheBytes == nullptr) || (gcCacheBytes->Length < 1))
        {
            cacheAccepted = false;
            return Compile(gcDocumentName, gcCode);
        }

        try
        {
            auto length = gcCacheBytes->Length;
            std::vector<std::uint8_t> cacheBytes(length);
            Marshal::Copy(gcCacheBytes, 0, (IntPtr)&cacheBytes[0], length);

            bool tempCacheAccepted;
            auto cacheType = (cacheKind == V8CacheKind::Parser) ? V8CacheType::Parser : V8CacheType::Code;
            auto gcScript = gcnew V8ScriptImpl(gcDocumentName, GetIsolate()->Compile(StdString(gcDocumentName), StdString(gcCode), cacheType, cacheBytes, tempCacheAccepted));

            cacheAccepted = tempCacheAccepted;
            return gcScript;
        }
        catch (const V8Exception& exception)
        {
            exception.ThrowScriptEngineException();
        }
    }

    //-------------------------------------------------------------------------

    V8RuntimeHeapInfo^ V8IsolateProxyImpl::GetHeapInfo()
    {
        V8IsolateHeapInfo heapInfo;
        GetIsolate()->GetHeapInfo(heapInfo);

        auto gcHeapInfo = gcnew V8RuntimeHeapInfo();
        gcHeapInfo->TotalHeapSize = heapInfo.GetTotalHeapSize();
        gcHeapInfo->TotalHeapSizeExecutable = heapInfo.GetTotalHeapSizeExecutable();
        gcHeapInfo->TotalPhysicalSize = heapInfo.GetTotalPhysicalSize();
        gcHeapInfo->UsedHeapSize = heapInfo.GetUsedHeapSize();
        gcHeapInfo->HeapSizeLimit = heapInfo.GetHeapSizeLimit();
        return gcHeapInfo;
    }

    //-------------------------------------------------------------------------

    void V8IsolateProxyImpl::CollectGarbage(bool exhaustive)
    {
        GetIsolate()->CollectGarbage(exhaustive);
    }

    //-------------------------------------------------------------------------

    SharedPtr<V8Isolate> V8IsolateProxyImpl::GetIsolate()
    {
        BEGIN_LOCK_SCOPE(m_gcLock)

            if (m_pspIsolate == nullptr)
            {
                throw gcnew ObjectDisposedException(ToString());
            }

            return *m_pspIsolate;

        END_LOCK_SCOPE
    }

    //-------------------------------------------------------------------------

    V8IsolateProxyImpl::~V8IsolateProxyImpl()
    {
        SharedPtr<V8Isolate> spIsolate;

        BEGIN_LOCK_SCOPE(m_gcLock)

            if (m_pspIsolate != nullptr)
            {
                // hold V8 isolate for destruction outside lock scope
                spIsolate = *m_pspIsolate;
                delete m_pspIsolate;
                m_pspIsolate = nullptr;
            }

        END_LOCK_SCOPE

        if (!spIsolate.IsEmpty())
        {
            GC::SuppressFinalize(this);
        }
    }

    //-------------------------------------------------------------------------

    V8IsolateProxyImpl::!V8IsolateProxyImpl()
    {
        if (m_pspIsolate != nullptr)
        {
            delete m_pspIsolate;
            m_pspIsolate = nullptr;
        }
    }

    //-------------------------------------------------------------------------

    int V8IsolateProxyImpl::AdjustConstraint(int value)
    {
        const int maxValueInMiB = 1024 * 1024;
        if (value > maxValueInMiB)
        {
            const double bytesPerMiB = 1024 * 1024;
            return Convert::ToInt32(Math::Ceiling(Convert::ToDouble(value) / bytesPerMiB));
        }

        return value;
    }

    //-------------------------------------------------------------------------

    ENSURE_INTERNAL_CLASS(V8IsolateProxyImpl)

}}}
