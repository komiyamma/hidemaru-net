// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Microsoft.ClearScript.JavaScript;
using Microsoft.ClearScript.Util;

namespace Microsoft.ClearScript
{
    /// <summary>
    /// Represents a document access configuration.
    /// </summary>
    /// <remarks>
    /// This class can be extended to accommodate custom document loaders.
    /// </remarks>
    public class DocumentSettings
    {
        private DocumentLoader loader;
        private readonly ConcurrentDictionary<Tuple<string, DocumentCategory>, Document> systemDocumentMap = new ConcurrentDictionary<Tuple<string, DocumentCategory>, Document>();

        // ReSharper disable EmptyConstructor

        /// <summary>
        /// Initializes a new <see cref="DocumentSettings"/> instance.
        /// </summary>
        public DocumentSettings()
        {
            // the help file builder (SHFB) insists on an empty constructor here
        }

        // ReSharper restore EmptyConstructor

        /// <summary>
        /// Gets or sets a document loader.
        /// </summary>
        public DocumentLoader Loader
        {
            get { return loader ?? DocumentLoader.Default; }
            set { loader = value; }
        }

        /// <summary>
        /// Gets or sets document access options.
        /// </summary>
        public DocumentAccessFlags AccessFlags { get; set; }

        /// <summary>
        /// Gets or sets a semicolon-delimited list of directory URLs or paths to search for documents.
        /// </summary>
        public string SearchPath { get; set; }

        /// <summary>
        /// Gets or sets a semicolon-delimited list of supported file name extensions.
        /// </summary>
        public string FileNameExtensions { get; set; }

        /// <summary>
        /// Gets or set an optional method to be called when a document is loaded.
        /// </summary>
        public DocumentLoadCallback LoadCallback { get; set; }

        /// <summary>
        /// Gets or sets an optional document context callback.
        /// </summary>
        /// <remarks>
        /// <para>
        /// This property is used as an alternative to <see cref="DocumentInfo.ContextCallback"/>.
        /// If specified, the callback is invoked the first time a module attempts to retrieve its
        /// context information. The properties it returns are made available to the module
        /// implementation. This mechanism can be used to expose host resources selectively,
        /// securely, and without polluting the script engine's global namespace.
        /// </para>
        /// <para>
        /// Use 
        /// <c><see href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/import.meta">import.meta</see></c>
        /// to access the context information of a <see cref="ModuleCategory.Standard"/> JavaScript
        /// module. In a <see cref="ModuleCategory.CommonJS"/> module, use <c>module.meta</c>.
        /// </para>
        /// </remarks>
        public DocumentContextCallback ContextCallback { get; set; }

        /// <summary>
        /// Adds a system document to the configuration.
        /// </summary>
        /// <param name="identifier">An identifier for the document.</param>
        /// <param name="contents">A string containing the document's contents.</param>
        /// <remarks>
        /// System documents take precedence over loaded documents. Once this method is invoked,
        /// document access using this configuration will always map the combination of
        /// <paramref name="identifier"/> and <see cref="DocumentCategory.Script"/> to the
        /// specified document, bypassing the configuration's document loader.
        /// </remarks>
        public void AddSystemDocument(string identifier, string contents)
        {
            AddSystemDocument(identifier, null, contents);
        }

        /// <summary>
        /// Adds a system document with the specified category to the configuration.
        /// </summary>
        /// <param name="identifier">An identifier for the document.</param>
        /// <param name="category">An optional category for the document.</param>
        /// <param name="contents">A string containing the document's contents.</param>
        /// <remarks>
        /// System documents take precedence over loaded documents. Once this method is invoked,
        /// document access using this configuration will always map the combination of
        /// <paramref name="identifier"/> and <paramref name="category"/> to the specified
        /// document, bypassing the configuration's document loader.
        /// </remarks>
        public void AddSystemDocument(string identifier, DocumentCategory category, string contents)
        {
            AddSystemDocument(identifier, category, contents, null);
        }

        /// <summary>
        /// Adds a system document with the specified category and context callback to the configuration.
        /// </summary>
        /// <param name="identifier">An identifier for the document.</param>
        /// <param name="category">An optional category for the document.</param>
        /// <param name="contents">A string containing the document's contents.</param>
        /// <param name="contextCallback">An optional context callback for the document.</param>
        /// <remarks>
        /// System documents take precedence over loaded documents. Once this method is invoked,
        /// document access using this configuration will always map the combination of
        /// <paramref name="identifier"/> and <paramref name="category"/> to the specified
        /// document, bypassing the configuration's document loader.
        /// </remarks>
        public void AddSystemDocument(string identifier, DocumentCategory category, string contents, DocumentContextCallback contextCallback)
        {
            MiscHelpers.VerifyNonBlankArgument(identifier, "specifier", "Invalid document specifier");
            var info = new DocumentInfo(Path.GetFileName(identifier)) { Category = category, ContextCallback = contextCallback };
            AddSystemDocument(identifier, new StringDocument(info, contents));
        }


        /// <summary>
        /// Adds the specified document as a system document to the configuration.
        /// </summary>
        /// <param name="identifier">An identifier for the document.</param>
        /// <param name="document">The document to be added as a system document.</param>
        /// <remarks>
        /// System documents take precedence over loaded documents. Once this method is invoked,
        /// document access using this configuration will always map the combination of
        /// <paramref name="identifier"/> and the specified document's category to the specified
        /// document, bypassing the configuration's document loader.
        /// </remarks>
        public void AddSystemDocument(string identifier, Document document)
        {
            MiscHelpers.VerifyNonNullArgument(document, "document");
            systemDocumentMap[Tuple.Create(identifier, document.Info.Category)] = document;
        }

        internal Document LoadDocument(DocumentInfo? sourceInfo, string specifier, DocumentCategory category, DocumentContextCallback contextCallback)
        {
            var document = FindSystemDocument(specifier, category);
            if (document != null)
            {
                return document;
            }

            document = Loader.LoadDocument(this, sourceInfo, specifier, category, contextCallback);
            if (document.Info.Category != (category ?? DocumentCategory.Script))
            {
                throw new FileLoadException("Loaded document category mismatch", "specifier");
            }

            return document;
        }

        internal async Task<Document> LoadDocumentAsync(DocumentInfo? sourceInfo, string specifier, DocumentCategory category, DocumentContextCallback contextCallback)
        {
            var document = FindSystemDocument(specifier, category);
            if (document != null)
            {
                return document;
            }

            document = await Loader.LoadDocumentAsync(this, sourceInfo, specifier, category, contextCallback).ConfigureAwait(false);
            if (document.Info.Category != (category ?? DocumentCategory.Script))
            {
                throw new FileLoadException("Loaded document category mismatch", "specifier");
            }

            return document;
        }

        private Document FindSystemDocument(string identifier, DocumentCategory category)
        {
            try
            {
                return systemDocumentMap[Tuple.Create(identifier, category ?? DocumentCategory.Script)];
            }
            catch (KeyNotFoundException)
            {
                return null;
            }
        }
    }
}
