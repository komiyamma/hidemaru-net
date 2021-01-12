// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Threading.Tasks;
using Microsoft.ClearScript.Util;

namespace Microsoft.ClearScript.JavaScript
{
    /// <summary>
    /// Defines extension methods for use with JavaScript engines.
    /// </summary>
    public static class JavaScriptExtensions
    {
        /// <summary>
        /// Converts a <see cref="Task{TResult}"/> instance to a
        /// <see href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise">promise</see>
        /// for use with script code currently running on the calling thread.
        /// </summary>
        /// <typeparam name="TResult">The task's result type.</typeparam>
        /// <param name="task">The task to convert to a promise.</param>
        /// <returns>A promise that represents the task's asynchronous operation.</returns>
        public static object ToPromise<TResult>(this Task<TResult> task)
        {
            return task.ToPromise(ScriptEngine.Current);
        }

        /// <summary>
        /// Converts a <see cref="Task{TResult}"/> instance to a
        /// <see href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise">promise</see>
        /// for use with script code running in the specified script engine.
        /// </summary>
        /// <typeparam name="TResult">The task's result type.</typeparam>
        /// <param name="task">The task to convert to a promise.</param>
        /// <param name="engine">The script engine in which the promise will be used.</param>
        /// <returns>A promise that represents the task's asynchronous operation.</returns>
        public static object ToPromise<TResult>(this Task<TResult> task, ScriptEngine engine)
        {
            MiscHelpers.VerifyNonNullArgument(task, "task");
            MiscHelpers.VerifyNonNullArgument(engine, "engine");

            var javaScriptEngine = engine as IJavaScriptEngine;
            if ((javaScriptEngine == null) || (javaScriptEngine.BaseLanguageVersion < 6))
            {
                throw new NotSupportedException("The script engine does not support promises");
            }

            return javaScriptEngine.CreatePromiseForTask(task);
        }

        /// <summary>
        /// Converts a <see cref="Task"/> instance to a
        /// <see href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise">promise</see>
        /// for use with script code currently running on the calling thread.
        /// </summary>
        /// <param name="task">The task to convert to a promise.</param>
        /// <returns>A promise that represents the task's asynchronous operation.</returns>
        public static object ToPromise(this Task task)
        {
            return task.ToPromise(ScriptEngine.Current);
        }

        /// <summary>
        /// Converts a <see cref="Task"/> instance to a
        /// <see href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise">promise</see>
        /// for use with script code running in the specified script engine.
        /// </summary>
        /// <param name="task">The task to convert to a promise.</param>
        /// <param name="engine">The script engine in which the promise will be used.</param>
        /// <returns>A promise that represents the task's asynchronous operation.</returns>
        public static object ToPromise(this Task task, ScriptEngine engine)
        {
            MiscHelpers.VerifyNonNullArgument(task, "task");
            MiscHelpers.VerifyNonNullArgument(engine, "engine");

            var javaScriptEngine = engine as IJavaScriptEngine;
            if ((javaScriptEngine == null) || (javaScriptEngine.BaseLanguageVersion < 6))
            {
                throw new NotSupportedException("The script engine does not support promises");
            }

            return javaScriptEngine.CreatePromiseForTask(task);
        }

        /// <summary>
        /// Converts a
        /// <see href="https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Promise">promise</see>
        /// to a <see cref="Task{Object}"/> instance.
        /// </summary>
        /// <param name="promise">The promise to convert to a task.</param>
        /// <returns>A task that represents the promise's asynchronous operation.</returns>
        public static Task<object> ToTask(this object promise)
        {
            MiscHelpers.VerifyNonNullArgument(promise, "promise");

            var scriptObject = promise as ScriptObject;
            if (scriptObject == null)
            {
                throw new ArgumentException("The object is not a promise", nameof(promise));
            }

            var javaScriptEngine = scriptObject.Engine as IJavaScriptEngine;
            if ((javaScriptEngine == null) || (javaScriptEngine.BaseLanguageVersion < 6))
            {
                throw new NotSupportedException("The script engine does not support promises");
            }

            return javaScriptEngine.CreateTaskForPromise(scriptObject);
        }
    }
}
