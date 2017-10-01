// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

//-----------------------------------------------------------------------------
// RefCount
//-----------------------------------------------------------------------------

class RefCount
{
    PROHIBIT_COPY(RefCount)

public:

    explicit RefCount(size_t count);

    size_t Increment();
    size_t Decrement();

    ~RefCount();

private:

    class RefCountImpl* m_pImpl;
};
