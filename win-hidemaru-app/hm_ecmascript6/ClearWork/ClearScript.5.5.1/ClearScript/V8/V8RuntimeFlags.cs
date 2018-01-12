// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;

namespace Microsoft.ClearScript.V8
{
    /// <summary>
    /// Defines options for initializing a new V8 runtime instance.
    /// </summary>
    [Flags]
    public enum V8RuntimeFlags
    {
        /// <summary>
        /// Specifies that no options are selected.
        /// </summary>
        None = 0,

        /// <summary>
        /// Specifies that script debugging features are to be enabled.
        /// </summary>
        EnableDebugging = 0x00000001,

        /// <summary>
        /// Specifies that remote script debugging is to be enabled. This option is ignored if
        /// <see cref="EnableDebugging"/> is not specified.
        /// </summary>
        EnableRemoteDebugging = 0x00000002
    }
}
