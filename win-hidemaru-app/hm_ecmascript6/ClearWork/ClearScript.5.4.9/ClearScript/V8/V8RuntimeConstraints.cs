﻿// 
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
// Microsoft Public License (MS-PL)
// 
// This license governs use of the accompanying software. If you use the
// software, you accept this license. If you do not accept the license, do not
// use the software.
// 
// 1. Definitions
// 
//   The terms "reproduce," "reproduction," "derivative works," and
//   "distribution" have the same meaning here as under U.S. copyright law. A
//   "contribution" is the original software, or any additions or changes to
//   the software. A "contributor" is any person that distributes its
//   contribution under this license. "Licensed patents" are a contributor's
//   patent claims that read directly on its contribution.
// 
// 2. Grant of Rights
// 
//   (A) Copyright Grant- Subject to the terms of this license, including the
//       license conditions and limitations in section 3, each contributor
//       grants you a non-exclusive, worldwide, royalty-free copyright license
//       to reproduce its contribution, prepare derivative works of its
//       contribution, and distribute its contribution or any derivative works
//       that you create.
// 
//   (B) Patent Grant- Subject to the terms of this license, including the
//       license conditions and limitations in section 3, each contributor
//       grants you a non-exclusive, worldwide, royalty-free license under its
//       licensed patents to make, have made, use, sell, offer for sale,
//       import, and/or otherwise dispose of its contribution in the software
//       or derivative works of the contribution in the software.
// 
// 3. Conditions and Limitations
// 
//   (A) No Trademark License- This license does not grant you rights to use
//       any contributors' name, logo, or trademarks.
// 
//   (B) If you bring a patent claim against any contributor over patents that
//       you claim are infringed by the software, your patent license from such
//       contributor to the software ends automatically.
// 
//   (C) If you distribute any portion of the software, you must retain all
//       copyright, patent, trademark, and attribution notices that are present
//       in the software.
// 
//   (D) If you distribute any portion of the software in source code form, you
//       may do so only under this license by including a complete copy of this
//       license with your distribution. If you distribute any portion of the
//       software in compiled or object code form, you may only do so under a
//       license that complies with this license.
// 
//   (E) The software is licensed "as-is." You bear the risk of using it. The
//       contributors give no express warranties, guarantees or conditions. You
//       may have additional consumer rights under your local laws which this
//       license cannot change. To the extent permitted under your local laws,
//       the contributors exclude the implied warranties of merchantability,
//       fitness for a particular purpose and non-infringement.
//       

using System;

namespace Microsoft.ClearScript.V8
{
    /// <summary>
    /// Specifies resource constraints for a V8 runtime.
    /// </summary>
    public sealed class V8RuntimeConstraints
    {
        // ReSharper disable EmptyConstructor

        /// <summary>
        /// Initializes a new <see cref="V8RuntimeConstraints"/> instance.
        /// </summary>
        public V8RuntimeConstraints()
        {
            // the help file builder (SHFB) insists on an empty constructor here
        }

        // ReSharper restore EmptyConstructor

        /// <summary>
        /// Gets or sets the maximum size of the new object heap in
        /// <see href="http://en.wikipedia.org/wiki/Mebibyte">MiB</see>.
        /// </summary>
        /// <remarks>
        /// For maximum compatibility with hosts that predate an inadvertent breaking change in
        /// ClearScript 5.4.1, values greater than 1048576
        /// (1 <see href="http://en.wikipedia.org/wiki/Tebibyte">TiB</see>) are assumed to be in
        /// bytes rather than MiB. For example, the values 16 and 16777216 both specify a limit
        /// of 16 MiB.
        /// </remarks>
        public int MaxNewSpaceSize { get; set; }

        /// <summary>
        /// Gets or sets the maximum size of the old object heap in
        /// <see href="http://en.wikipedia.org/wiki/Mebibyte">MiB</see>.
        /// </summary>
        /// <remarks>
        /// For maximum compatibility with hosts that predate an inadvertent breaking change in
        /// ClearScript 5.4.1, values greater than 1048576
        /// (1 <see href="http://en.wikipedia.org/wiki/Tebibyte">TiB</see>) are assumed to be in
        /// bytes rather than MiB. For example, the values 16 and 16777216 both specify a limit
        /// of 16 MiB.
        /// </remarks>
        public int MaxOldSpaceSize { get; set; }

        /// <summary>
        /// Gets or sets the maximum size of the executable code heap in
        /// <see href="http://en.wikipedia.org/wiki/Mebibyte">MiB</see>.
        /// </summary>
        /// <remarks>
        /// For maximum compatibility with hosts that predate an inadvertent breaking change in
        /// ClearScript 5.4.1, values greater than 1048576
        /// (1 <see href="http://en.wikipedia.org/wiki/Tebibyte">TiB</see>) are assumed to be in
        /// bytes rather than MiB. For example, the values 16 and 16777216 both specify a limit
        /// of 16 MiB.
        /// </remarks>
        public int MaxExecutableSize { get; set; }

        /// <summary>
        /// Gets or sets the maximum size of the young object heap in
        /// <see href="http://en.wikipedia.org/wiki/Mebibyte">MiB</see>.
        /// </summary>
        /// <remarks>
        /// For maximum compatibility with hosts that predate an inadvertent breaking change in
        /// ClearScript 5.4.1, values greater than 1048576
        /// (1 <see href="http://en.wikipedia.org/wiki/Tebibyte">TiB</see>) are assumed to be in
        /// bytes rather than MiB. For example, the values 16 and 16777216 both specify a limit
        /// of 16 MiB.
        /// </remarks>
        [Obsolete("This property is obsolete. Use MaxNewSpaceSize instead.")]
        public int MaxYoungSpaceSize
        {
            get { return MaxNewSpaceSize; }
            set { MaxNewSpaceSize = value; }
        }
    }
}
