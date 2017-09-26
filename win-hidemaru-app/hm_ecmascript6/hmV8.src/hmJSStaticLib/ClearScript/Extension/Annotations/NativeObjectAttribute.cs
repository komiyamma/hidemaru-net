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
