/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;

namespace hmV8DynamicNS
{
    public class NativeObjectAttribute : Attribute
    {
        public NativeObjectAttribute(string nativeType = null)
        {
            NativeType = nativeType;
        }

        public string NativeType { get; }
    }
}
