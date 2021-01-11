// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

namespace Microsoft.ClearScript
{
    /// <summary>
    /// Defines properties that comprise ClearScript's global configuration.
    /// </summary>
    public static class HostSettings
    {
        /// <summary>
        /// Enables or disables assembly table usage.
        /// </summary>
        /// <remarks>
        ///<para>
        /// The assembly table is a legacy internal feature intended to accelerate assembly
        /// loading. Because it relies on deprecated platform functionality, this feature is now
        /// disabled by default. Although its replacement is simpler and more efficient, the
        /// feature is still available to provide full compatibility with older ClearScript
        /// releases.
        /// </para>
        ///<para>
        /// The assembly table feature is only available on .NET Framework. This property has no
        /// effect on other platforms.
        /// </para>
        /// </remarks>
        public static bool UseAssemblyTable { get; set; }
    }
}
