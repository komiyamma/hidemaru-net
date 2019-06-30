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
// standard library headers
//-----------------------------------------------------------------------------

#include <algorithm>
#include <cstdint>
#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

//-----------------------------------------------------------------------------
// global macros
//-----------------------------------------------------------------------------

#define PROHIBIT_CONSTRUCT(CLASS) \
    private: \
        CLASS();

//-----------------------------------------------------------------------------

#define PROHIBIT_HEAP(CLASS) \
    private: \
        void* operator new(size_t size); \
        void operator delete(void*, size_t);

//-----------------------------------------------------------------------------

#define PROHIBIT_COPY(CLASS) \
    private: \
        CLASS(const CLASS& that); \
        const CLASS& operator=(const CLASS& that);

//-----------------------------------------------------------------------------

#define IGNORE_UNUSED(NAME) ((void)(NAME))

#ifdef _DEBUG
    #define ASSERT_EVAL _ASSERTE
    #define DEBUG_EVAL IGNORE_UNUSED
#else // !_DEBUG
    #define ASSERT_EVAL IGNORE_UNUSED
    #define DEBUG_EVAL _ASSERTE
#endif // !_DEBUG

//-----------------------------------------------------------------------------

#ifndef DECLSPEC_NORETURN
    #define DECLSPEC_NORETURN __declspec(noreturn)
#endif // !DECLSPEC_NORETURN

//-----------------------------------------------------------------------------

#define BEGIN_COMPOUND_MACRO \
    do \
    {

#define END_COMPOUND_MACRO \
    __pragma(warning(disable:4127)) /* conditional expression is constant */ \
    } \
    while (false) \
    __pragma(warning(default:4127))

//-----------------------------------------------------------------------------
// global helper functions
//-----------------------------------------------------------------------------

template <typename TFlag>
inline TFlag CombineFlags(TFlag flag1, TFlag flag2)
{
    using TUnderlying = std::underlying_type_t<TFlag>;
    return static_cast<TFlag>(static_cast<TUnderlying>(flag1) | static_cast<TUnderlying>(flag2));
}

//-----------------------------------------------------------------------------

template <typename TFlag, typename... TOthers>
inline TFlag CombineFlags(TFlag flag1, TFlag flag2, TOthers... others)
{
    return CombineFlags(flag1, CombineFlags(flag2, others...));
}

//-----------------------------------------------------------------------------
// PulseValueScope
//-----------------------------------------------------------------------------

template <typename T>
class PulseValueScope
{
    PROHIBIT_COPY(PulseValueScope)
    PROHIBIT_HEAP(PulseValueScope)

public:

    PulseValueScope(T* pValue, T&& value):
        m_pValue(pValue),
        m_OriginalValue(std::move(*pValue))
    {
        *m_pValue = std::move(value);
    }

    ~PulseValueScope()
    {
        *m_pValue = std::move(m_OriginalValue);
    }

private:

    T* m_pValue;
    T m_OriginalValue;
};

//-----------------------------------------------------------------------------

#define BEGIN_PULSE_VALUE_SCOPE(ADDRESS, VALUE) \
    { \
    __pragma(warning(disable:4456)) /* declaration hides previous local declaration */ \
        PulseValueScope<std::remove_reference<decltype(*(ADDRESS))>::type> t_PulseValueScope((ADDRESS), (VALUE)); \
    __pragma(warning(default:4456))

#define END_PULSE_VALUE_SCOPE \
        IGNORE_UNUSED(t_PulseValueScope); \
    }
