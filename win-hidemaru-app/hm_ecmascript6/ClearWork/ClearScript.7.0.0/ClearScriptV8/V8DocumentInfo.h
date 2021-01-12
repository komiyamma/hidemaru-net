// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

struct V8DocumentInfo final
{
public:

    V8DocumentInfo():
        m_IsModule(false),
        m_pvDocumentInfo(nullptr)
    {
    }

    V8DocumentInfo(const StdChar* pResourceName, const StdChar* pSourceMapUrl, uint64_t uniqueId, bool isModule, void* pvDocumentInfo):
        m_ResourceName(pResourceName),
        m_SourceMapUrl(pSourceMapUrl),
        m_UniqueId(uniqueId),
        m_IsModule(isModule),
        m_pvDocumentInfo(pvDocumentInfo)
    {
    }

    V8DocumentInfo(StdString&& resourceName, StdString&& sourceMapUrl, uint64_t uniqueId, bool isModule, void* pvDocumentInfo):
        m_ResourceName(std::move(resourceName)),
        m_SourceMapUrl(std::move(sourceMapUrl)),
        m_UniqueId(uniqueId),
        m_IsModule(isModule),
        m_pvDocumentInfo(pvDocumentInfo)
    {
    }

    V8DocumentInfo(const V8DocumentInfo& that):
        m_ResourceName(that.m_ResourceName),
        m_SourceMapUrl(that.m_SourceMapUrl),
        m_UniqueId(that.m_UniqueId),
        m_IsModule(that.m_IsModule),
        m_pvDocumentInfo((that.m_pvDocumentInfo != nullptr) ? HostObjectUtil::GetInstance().AddRef(that.m_pvDocumentInfo) : nullptr)
    {
    }

    V8DocumentInfo(V8DocumentInfo&& that) noexcept:
        m_ResourceName(std::move(that.m_ResourceName)),
        m_SourceMapUrl(std::move(that.m_SourceMapUrl)),
        m_UniqueId(that.m_UniqueId),
        m_IsModule(that.m_IsModule),
        m_pvDocumentInfo(that.m_pvDocumentInfo)
    {
        that.m_pvDocumentInfo = nullptr;
    }

    const V8DocumentInfo& operator=(const V8DocumentInfo& that)
    {
        V8DocumentInfo tempInfo(std::move(*this));
        m_ResourceName = that.m_ResourceName;
        m_SourceMapUrl = that.m_SourceMapUrl;
        m_UniqueId = that.m_UniqueId;
        m_IsModule = that.m_IsModule;
        m_pvDocumentInfo = (that.m_pvDocumentInfo != nullptr) ? HostObjectUtil::GetInstance().AddRef(that.m_pvDocumentInfo) : nullptr;
        return *this;
    }

    const V8DocumentInfo& operator=(V8DocumentInfo&& that) noexcept
    {
        V8DocumentInfo tempInfo(std::move(*this));
        m_ResourceName = std::move(that.m_ResourceName);
        m_SourceMapUrl = std::move(that.m_SourceMapUrl);
        m_UniqueId = that.m_UniqueId;
        m_IsModule = that.m_IsModule;
        m_pvDocumentInfo = that.m_pvDocumentInfo;
        that.m_pvDocumentInfo = nullptr;
        return *this;
    }

    ~V8DocumentInfo()
    {
        if (m_pvDocumentInfo != nullptr)
        {
            HostObjectUtil::GetInstance().Release(m_pvDocumentInfo);
        }
    }

    const StdString& GetResourceName() const { return m_ResourceName; }
    const StdString& GetSourceMapUrl() const { return m_SourceMapUrl; }
    uint64_t GetUniqueId() const { return m_UniqueId; }
    bool IsModule() const { return m_IsModule; }
    void* GetDocumentInfo() const { return m_pvDocumentInfo; }

private:

    StdString m_ResourceName;
    StdString m_SourceMapUrl;
    uint64_t m_UniqueId {};
    bool m_IsModule;
    void* m_pvDocumentInfo;
};
