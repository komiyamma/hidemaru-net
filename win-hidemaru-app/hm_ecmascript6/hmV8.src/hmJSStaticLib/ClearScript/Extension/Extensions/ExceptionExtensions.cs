/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using System;
using Microsoft.ClearScript;
using hmV8DynamicNS.Helpers;

namespace hmV8DynamicNS.Extensions
{
    public static class ExceptionExtensions
    {
        public static T GetInnerMost<T>(this Exception exception) where T : class
        {
            var testEx = exception;
            T result = null;
            while (testEx != null)
            {
                var targetEx = testEx as T;
                if (targetEx != null)
                {
                    result = targetEx;
                }

                testEx = testEx.InnerException;
            }

            return result;
        }

        public static string GetScriptStack(this Exception exception)
        {
            var error = exception.GetInnerMost<IScriptEngineException>()?.ErrorDetails;
            return error != null ? StringHelpers.CleanupStackTrace(error) : exception.Message;
        }
    }
}
