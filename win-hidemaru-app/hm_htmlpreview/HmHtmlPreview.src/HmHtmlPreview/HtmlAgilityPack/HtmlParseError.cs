// HtmlAgilityPack V1.0 - Simon Mourier <simon underscore mourier at hotmail dot com>
namespace HtmlAgilityPack
{
    /// <summary>
    /// Represents a parsing error found during document parsing.
    /// </summary>
    public class HtmlParseError
    {
        internal HtmlParseError(HtmlParseErrorCode code, int line, int linePosition,
            int streamPosition, string sourceText, string reason)
        {
            Code = code;
            Line = line;
            LinePosition = linePosition;
            StreamPosition = streamPosition;
            SourceText = sourceText;
            Reason = reason;
        }

        /// <summary>
        /// Gets the type of error.
        /// </summary>
        public HtmlParseErrorCode Code { get; private set; }

        /// <summary>
        /// Gets the line number of this error in the document.
        /// </summary>
        public int Line { get; private set; }

        /// <summary>
        /// Gets the column number of this error in the document.
        /// </summary>
        public int LinePosition { get; private set; }

        /// <summary>
        /// Gets a description for the error.
        /// </summary>
        public string Reason { get; private set; }

        /// <summary>
        /// Gets the the full text of the line containing the error.
        /// </summary>
        public string SourceText { get; private set; }

        /// <summary>
        /// Gets the absolute stream position of this error in the document, relative to the start of the document.
        /// </summary>
        public int StreamPosition { get; private set; }
    }
}