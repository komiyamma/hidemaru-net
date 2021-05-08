using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp16
{
    class _AsFunction : DynamicObject
    {
        public override bool TryInvokeMember( InvokeMemberBinder binder, object[] args, out object result)
        {
            Console.WriteLine(binder.Name);

            int count = args.Length;
            Console.WriteLine(count);

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
        public override bool TryInvokeMember(InvokeMemberBinder binder, object[] args, out object result)
        {
            Console.WriteLine(binder.Name);

            int count = args.Length;
            Console.WriteLine(count);

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
        static dynamic AsFunc = new _AsFunction();
        static dynamic AsState = new _AsStatement();

        static void Main(string[] args)
        {
            AsFunc.message(1, "34", 3);
            AsState.keep(3, 2);

            double a = -3.3;
            Console.WriteLine(Math.Floor(a));
        }
    }

}
