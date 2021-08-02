﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

namespace Microsoft.ClearScript
{
    internal enum Invocability
    {
        // IMPORTANT: maintain bitwise equivalence with native enum IHostObjectUtil::Invocability
        None,
        Delegate,
        Dynamic,
        DefaultProperty
    }
}
