// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

namespace Microsoft.ClearScript.V8
{
    internal class V8ArrayBufferOrViewInfo
    {
        public V8ArrayBufferOrViewInfo(V8ArrayBufferOrViewKind kind, IV8Object arrayBuffer, ulong offset, ulong size, ulong length)
        {
            Kind = kind;
            ArrayBuffer = arrayBuffer;
            Offset = offset;
            Size = size;
            Length = length;
        }

        public V8ArrayBufferOrViewKind Kind { get; private set; }

        public IV8Object ArrayBuffer { get; private set; }

        public ulong Offset { get; private set; }

        public ulong Size { get; private set; }

        public ulong Length { get; private set; }
    }
}
