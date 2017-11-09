// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

using System;
using System.Diagnostics;
using System.IO;
using Microsoft.ClearScript.V8;

namespace Microsoft.ClearScript.Test
{
    internal static class ClearScriptConsole
    {
        public static void Main()
        {
            using (var engine = new V8ScriptEngine(typeof(ClearScriptConsole).Name, V8ScriptEngineFlags.EnableDebugging))
            {
                engine.AddHostObject("host", new ExtendedHostFunctions());
                engine.AddHostObject("lib", HostItemFlags.GlobalMembers, new HostTypeCollection("mscorlib", "System", "System.Core", "ClearScript"));
                engine.AllowReflection = true;

                RunStartupFile(engine);
                RunConsole(engine);
            }
        }

        private static void RunStartupFile(ScriptEngine engine)
        {
            try
            {
                // ReSharper disable AssignNullToNotNullAttribute

                var fileName = Path.ChangeExtension("Startup", engine.FileNameExtension);
                var filePath = Path.Combine(Path.GetDirectoryName(Process.GetCurrentProcess().MainModule.FileName), fileName);
                if (File.Exists(filePath))
                {
                    engine.Execute(fileName, File.ReadAllText(filePath));
                }

                // ReSharper restore AssignNullToNotNullAttribute
            }
            catch (Exception exception)
            {
                Console.WriteLine("Error: {0}", exception.GetBaseException().Message);
            }
        }

        private static void RunConsole(ScriptEngine engine)
        {
            while (true)
            {
                Console.Write("-> ");

                var command = Console.ReadLine();
                if (command == null)
                {
                    break;
                }

                if (command.Trim().Length < 1)
                {
                    continue;
                }

                try
                {
                    var result = engine.ExecuteCommand(command);
                    if (result != null)
                    {
                        Console.WriteLine(result);
                    }
                }
                catch (Exception exception)
                {
                    Console.WriteLine("Error: {0}", exception.GetBaseException().Message);
                }
            }
        }
    }
}
