// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

namespace Microsoft.ClearScript
{
    /// <summary>
    /// Represents a document category.
    /// </summary>
    public abstract class DocumentCategory
    {
        internal DocumentCategory()
        {
            MaxCacheSize = 1024;
        }

        /// <summary>
        /// Gets or sets the maximum cache size for the document category.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This property specifies the maximum number of prepared or compiled documents of the
        /// current category to be cached by script engines. Its initial value is 1024.
        /// </para>
        /// <para>
        /// Each script engine or runtime maintains private caches for supported document
        /// categories. These are distinct from the caches used by document loaders.
        /// </para>
        /// </remarks>
        /// <seealso cref="DocumentLoader.MaxCacheSize"/>
        public uint MaxCacheSize { get; set; }

        /// <summary>
        /// Gets the document category for normal scripts.
        /// </summary>
        public static DocumentCategory Script => ScriptDocument.Instance;

        internal abstract string DefaultName { get; }

        #region Nested type: ScriptDocument

        private sealed class ScriptDocument : DocumentCategory
        {
            public static readonly ScriptDocument Instance = new ScriptDocument();

            private ScriptDocument()
            {
            }

            #region DocumentCategory overrides

            internal override string DefaultName => "Script";

            #endregion

            #region Object overrides

            public override string ToString()
            {
                return "Script";
            }

            #endregion
        }

        #endregion
    }
}
