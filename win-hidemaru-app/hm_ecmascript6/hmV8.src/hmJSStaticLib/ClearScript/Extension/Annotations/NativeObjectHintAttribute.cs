using System;

namespace hmV8DynamicNS
{
    public class NativeObjectHintAttribute : Attribute
    {
        public NativeObjectHintAttribute(string nativeType = null)
        {
            NativeType = nativeType;
        }

        public string NativeType { get; }
    }
}
