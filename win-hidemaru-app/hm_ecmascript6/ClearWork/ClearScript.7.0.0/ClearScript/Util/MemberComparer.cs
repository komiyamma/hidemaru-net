﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.Reflection;

namespace Microsoft.ClearScript.Util
{
    internal sealed class MemberComparer<T> : EqualityComparer<T> where T : MemberInfo
    {
        public static MemberComparer<T> Instance { get; } = new MemberComparer<T>();

        private MemberComparer()
        {
        }

        public override bool Equals(T x, T y)
        {
            try
            {
                return (x.Module == y.Module) && (x.MetadataToken == y.MetadataToken);
            }
            catch (Exception)
            {
                return x == y;
            }
        }

        public override int GetHashCode(T obj)
        {
            // ReSharper disable once ConditionIsAlwaysTrueOrFalse
            return (obj == null) ? 0 : obj.GetHashCode();
        }
    }
}
