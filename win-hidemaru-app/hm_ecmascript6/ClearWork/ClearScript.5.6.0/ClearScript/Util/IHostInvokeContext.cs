// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;

namespace Microsoft.ClearScript.Util
{
    internal interface IHostInvokeContext
    {
        ScriptEngine Engine { get; }
        Type AccessContext { get; }
        ScriptAccess DefaultAccess { get; }
    }
}
