using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp16
{
    class _AsFunction : DynamicObject
    {
        int base_random = 0;

        public _AsFunction()
        {
            base_random = new Random().Next(Int16.MaxValue);
        }
        public override bool TryInvokeMember( InvokeMemberBinder binder, object[] args, out object result)
        {
            var arg_list = new List<KeyValuePair<String, Object>>();
            int cur_random = new Random().Next(Int16.MaxValue);
            foreach (var arg in args)
            {
                cur_random++;
                if (arg is int)
                {
                    string key = "#AsFunction_" + base_random.ToString() + '_' + cur_random.ToString();
                    int value = (int)arg;
                    arg_list.Add(new KeyValuePair<string, object>(key, value));
                } else if (arg is string)
                {
                    string key = "$AsFunction_" + base_random.ToString() + '_' + cur_random.ToString();
                    string value = (string)arg;
                    arg_list.Add(new KeyValuePair<string, object>(key, value));

                }
            }
            Console.WriteLine(binder.Name);
            var arg_keys = new List<String>();
            foreach(var l in arg_list)
            {
                arg_keys.Add(l.Key);
            }

            int count = args.Length;
            foreach(var l in arg_list)
            {
                Console.WriteLine(l.ToString());
            }
            string args_string = String.Join(", ", arg_keys);
            string cmd = $"{binder.Name}({args_string})";
            Console.WriteLine(cmd);
            string var_command = cmd;
            String invocate = "3";
            String cmd2 = "" +
                "##_tmp_dll_id_ret = dllfuncw( " + invocate + " \"SetTmpVar\", " + var_command + ");\n" +
                "##_tmp_dll_id_ret = 0;\n";
            Console.WriteLine(cmd2);

            /*
            if ((args.Length == 2) &&
                (args[0].GetType() == typeof(int)) &&
                (args[1].GetType() == typeof(String)))
            {
            */
            if (true)
            {
                result = 3;
                return true;
            }

            else
            {
                result = "222";
                return false;
            }
        }
    }

    class _AsStatement : DynamicObject
    {
        int base_random = 0;
        public _AsStatement() {
            base_random = new Random().Next(Int16.MaxValue);
        }
        public override bool TryInvokeMember(InvokeMemberBinder binder, object[] args, out object result)
        {
            var arg_list = new List<KeyValuePair<String, Object>>();
            int cur_random = new Random().Next(Int16.MaxValue);
            foreach (var arg in args)
            {
                cur_random++;
                if (arg is int)
                {
                    string key = "#AsFunction_" + base_random.ToString() + '_' + cur_random.ToString();
                    int value = (int)arg;
                    arg_list.Add(new KeyValuePair<string, object>(key, value));
                }
                else if (arg is string)
                {
                    string key = "$AsFunction_" + base_random.ToString() + '_' + cur_random.ToString();
                    string value = (string)arg;
                    arg_list.Add(new KeyValuePair<string, object>(key, value));

                }
            }
            Console.WriteLine(binder.Name);
            var arg_keys = new List<String>();
            foreach (var l in arg_list)
            {
                arg_keys.Add(l.Key);
            }

            int count = args.Length;
            foreach (var l in arg_list)
            {
                Console.WriteLine(l.ToString());
            }
            string args_string = String.Join(", ", arg_keys);
            string cmd = $"{binder.Name} {args_string};\n";
            Console.WriteLine(cmd);
            /*
            if ((args.Length == 2) &&
                (args[0].GetType() == typeof(int)) &&
                (args[1].GetType() == typeof(String)))
            {
            */
            if (true)
            {
                result = 3;
                return true;
            }

            else
            {
                result = "222";
                return false;
            }
        }
    }

    class Program
    {
        static dynamic AsFn = new _AsFunction();
        static dynamic AsSt = new _AsStatement();

        static void Main(string[] args)
        {
            AsFn.message(1, "34", 3);
            AsSt.keep(3, 2);

            double a = -3.3;
            Console.WriteLine(Math.Floor(a));
        }
    }

}
