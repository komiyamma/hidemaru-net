// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Collections;
using System.Collections.Generic;
using Microsoft.ClearScript.Util;

namespace Microsoft.ClearScript
{
    internal interface IHostList
    {
        int Count { get; }
        object this[int index] { get; set; }
    }

    internal class HostList : IHostList
    {
        private readonly ScriptEngine engine;
        private readonly IList list;
        private readonly Type elementType;

        public HostList(ScriptEngine engine, IList list, Type elementType)
        {
            this.engine = engine;
            this.list = list;
            this.elementType = elementType;
        }

        #region IHostList implementation

        public int Count
        {
            get { return list.Count; }
        }

        public object this[int index]
        {
            get { return engine.PrepareResult(list[index], elementType, ScriptMemberFlags.None, true); }
            set { list[index] = value; }
        }

        #endregion
    }

    internal class HostList<T> : IHostList
    {
        private readonly ScriptEngine engine;
        private readonly IList<T> list;

        public HostList(ScriptEngine engine, IList<T> list)
        {
            this.engine = engine;
            this.list = list;
        }

        #region IHostList implementation

        public int Count
        {
            get { return list.Count; }
        }

        public object this[int index]
        {
            get { return engine.PrepareResult(list[index], ScriptMemberFlags.None, true); }

            set
            {
                if (!typeof(T).IsAssignableFrom(ref value))
                {
                    throw new InvalidOperationException("Assignment invalid due to type mismatch");
                }

                list[index] = (T)value;
            }
        }

        #endregion
    }
}
