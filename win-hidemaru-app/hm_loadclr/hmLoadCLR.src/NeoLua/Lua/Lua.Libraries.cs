﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Dynamic;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;

namespace Neo.IronLua
{
	#region -- String Manipulation ------------------------------------------------------

	///////////////////////////////////////////////////////////////////////////////
	/// <summary></summary>
	internal static class LuaLibraryString
	{
		private static bool lTranslateRegEx = true;

		private static void NormalizeStringArguments(string s, int i, int j, out int iStart, out int iLen)
		{
			if (i == 0)
				i = 1;

			if (i < 0) // Suffix mode
			{
				iStart = s.Length + i;
				if (iStart < 0)
					iStart = 0;
				iLen = (j < 0 ? s.Length + j + 1 : j) - iStart;
			}
			else // Prefix mode
			{
				iStart = i - 1;
				if (j < 0)
					j = s.Length + j + 1;
				iLen = j - iStart;
			}

			// correct the length
			if (iStart + iLen > s.Length)
				iLen = s.Length - iStart;
		} // proc NormalizeStringArguments

		private static string TranslateRegularExpression(string sRegEx)
		{
			if (!lTranslateRegEx)
				return sRegEx;

			StringBuilder sb = new StringBuilder();
			bool lEscape = false;

			for (int i = 0; i < sRegEx.Length; i++)
			{
				char c = sRegEx[i];
				if (lEscape)
				{
					if (c == '%')
					{
						sb.Append('%');
						lEscape = false;
					}
					else
					{
						switch (c)
						{
							case 'a': // all letters
								sb.Append("\\p{L}");
								break;
							case 'A': // all Non letters
								sb.Append("\\P{L}");
								break;
							
							case 's': // all space characters
								sb.Append("\\s");
								break;
							case 'S': // all NON space characters
								sb.Append("\\S");
								break;
							
							case 'd': // all digits
								sb.Append("\\d");
								break;
							case 'D': // all NON digits
								sb.Append("\\D");
								break;
							
							case 'w': // all alphanumeric characters
								sb.Append("\\w");
								break;
							case 'W': // all NON alphanumeric characters
								sb.Append("\\W");
								break;
							
							case 'c': // all control characters
								sb.Append("\\p{C}");
								break;
							case 'C': // all NON control characters
								sb.Append("[\\P{C}]");
								break;
							
							case 'g': // all printable characters except space
								sb.Append("[^\\p{C}\\s]");
								break;
							case 'G': // all NON printable characters including space
								sb.Append("[\\p{C}\\s]");
								break;
							
							case 'p': // all punctuation characters
								sb.Append("\\p{P}");
								break;
							case 'P': // all NON punctuation characters
								sb.Append("\\P{P}");
								break;
							
							case 'l': // all lowercase letters
								sb.Append("\\p{Ll}");
								break;
							case 'L': // all NON lowercase letters
								sb.Append("\\P{Ll}");
								break;
							
							case 'u': // all uppercase letters
								sb.Append("\\p{Lu}");
								break;
							case 'U': // all NON uppercase letters
								sb.Append("\\P{Lu}");
								break;
							
							case 'x': // all hexadecimal digits
								sb.Append("[0-9A-Fa-f]");
								break;
							case 'X': // all NON hexadecimal digits
								sb.Append("[^0-9A-Fa-f]");
								break;

							//case 'b': does not work
							//	if (i < sRegEx.Length - 2)
							//	{
							//		char c1 = sRegEx[i + 1];
							//		char c2 = sRegEx[i + 2];
							//		sb.Append("\\").Append(c1)
							//			.Append("(?:[^").Append(c1).Append(c2).Append("]")
							//				.Append("|(?<open> \\").Append(c1).Append(" )")
							//				.Append("|(?<-open> \\").Append(c2).Append(" )")
							//			.Append(")*")
							//			.Append("\\").Append(c2);
							//	}
							//	else
							//		throw new ArgumentOutOfRangeException();
							//	break;

							default:
								sb.Append('\\');
								sb.Append(c);
								break;
						}
						lEscape = false;
					}
				}
				else if (c == '%')
				{
					lEscape = true;
				}
				else if (c == '\\')
				{
					sb.Append("\\\\");
				}
				else if (c == '-')
					sb.Append("+?");
				else
					sb.Append(c);
			}

			return sb.ToString();
		} // func TranslateRegularExpression

		public static LuaResult @byte(this string s, Nullable<int> i = null, Nullable<int> j = null)
		{
			if (!i.HasValue)
				i = 1;
			if (!j.HasValue)
				j = i;

			if (String.IsNullOrEmpty(s) || i == 0)
				return LuaResult.Empty;

			int iStart;
			int iLen;
			NormalizeStringArguments(s, i.Value, j.Value, out iStart, out iLen);
			if (iLen <= 0)
				return LuaResult.Empty;

			object[] r = new object[iLen];
			for (int a = 0; a < iLen; a++)
				r[a] = (int)s[iStart + a];

			return r;
		} // func byte

		public static string @char(params int[] chars)
		{
			if (chars == null)
				return String.Empty;

			StringBuilder sb = new StringBuilder(chars.Length);
			for (int i = 0; i < chars.Length; i++)
				sb.Append((char)chars[i]);

			return sb.ToString();
		} // func char

		public static string dump(Delegate dlg)
		{
			throw new NotImplementedException();
		} // func dump

		public static LuaResult find(this string s, string pattern, int init = 1, bool plain = false)
		{
			if (String.IsNullOrEmpty(s))
			{
				if (String.IsNullOrEmpty(pattern) && init == 1)
					return new LuaResult(1);
				else
					return LuaResult.Empty;
			}
			if (String.IsNullOrEmpty(pattern))
				return LuaResult.Empty;

			// correct the init parameter
			if (init < 0)
				init = s.Length + init + 1;
			if (init <= 0)
				init = 1;

			if (plain) // plain pattern
			{
				int iIndex = s.IndexOf(pattern, init - 1);
				return new LuaResult(iIndex + 1, iIndex + pattern.Length);
			}
			else
			{
				// translate the regular expression
				pattern = TranslateRegularExpression(pattern);

				Regex r = new Regex(pattern);
				Match m = r.Match(s, init - 1);
				if (m.Success)
				{
					object[] result = new object[m.Captures.Count + 2];

					result[0] = m.Index + 1;
					result[1] = m.Index + m.Length;
					for (int i = 0; i < m.Captures.Count; i++)
						result[i + 2] = m.Captures[i].Value;

					return result;
				}
				else
					return LuaResult.Empty;
			}
		} // func find

		public static string format(this string formatstring, params object[] args)
		{
			return AT.MIN.Tools.sprintf(formatstring, args);
		} // func format

		private static LuaResult matchEnum(object s, object current)
		{
			System.Collections.IEnumerator e = (System.Collections.IEnumerator)s;

			// return value
			if (e.MoveNext())
			{
				Match m = (Match)e.Current;
				return MatchResult(m);
			}
			else
				return LuaResult.Empty;
		} // func matchEnum

		public static LuaResult gmatch(this string s, string pattern)
		{
			// f,s,v
			if (String.IsNullOrEmpty(s))
				return LuaResult.Empty;
			if (String.IsNullOrEmpty(pattern))
				return LuaResult.Empty;

			// translate the regular expression
			pattern = TranslateRegularExpression(pattern);

			// Find Matches
			System.Collections.IEnumerator e = Regex.Matches(s, pattern).GetEnumerator();
			return new LuaResult(new Func<object, object, LuaResult>(matchEnum), e, e);
		} // func gmatch

		#region -- class GSubMatchEvaluator -----------------------------------------------

		#region -- class GSubMatchEvaluator -----------------------------------------------

		///////////////////////////////////////////////////////////////////////////////
		/// <summary></summary>
		private abstract class GSubMatchEvaluator
		{
			private int iMatchCount = 0;

			public string MatchEvaluator(Match m)
			{
				iMatchCount++;
				return MatchEvaluatorImpl(m);
			} // func MatchEvaluator

			protected abstract string MatchEvaluatorImpl(Match m);

			public int MatchCount { get { return iMatchCount; } }
		} // class GSubMatchEvaluator

		#endregion

		#region -- class GSubLuaTableMatchEvaluator ---------------------------------------

		///////////////////////////////////////////////////////////////////////////////
		/// <summary></summary>
		private sealed class GSubLuaTableMatchEvaluator : GSubMatchEvaluator
		{
			private LuaTable t;
			private bool lIgnoreCase;

			public GSubLuaTableMatchEvaluator(LuaTable t)
			{
				this.t = t;
				this.lIgnoreCase = (bool)Lua.RtConvertValue(t.GetMemberValue("__IgnoreCase"), typeof(bool));
			} // ctor

			protected override string MatchEvaluatorImpl(Match m)
			{
				return (string)Lua.RtConvertValue(t.GetMemberValue(m.Groups[1].Value, lIgnoreCase), typeof(string));
			} // func MatchEvaluator
		} // class GSubLuaTableMatchEvaluator

		#endregion

		#region -- class GSubFunctionMatchEvaluator ---------------------------------------

		///////////////////////////////////////////////////////////////////////////////
		/// <summary></summary>
		private sealed class GSubFunctionMatchEvaluator : GSubMatchEvaluator
		{
			private CallSite callSite = null;
			private object funcCall;

			public GSubFunctionMatchEvaluator(object funcCall)
			{
				this.funcCall = funcCall;
			} // ctor

			private void UpdateCallSite(CallInfo callInfo, CallSite callSite)
			{
				this.callSite = callSite;
			} // proc UpdateCallSite

			protected override string MatchEvaluatorImpl(Match m)
			{
				string[] args = new string[m.Groups.Count - 1];
				for (int i = 1; i < m.Groups.Count; i++)
					args[i - 1] = m.Groups[i].Value;

				return (string)Lua.RtConvertValue(Lua.RtInvokeSite(callSite, callInfo => new Lua.LuaInvokeBinder(null, callInfo), UpdateCallSite, funcCall, args), typeof(string));
			} // func MatchEvaluator
		} // class GSubLuaTableMatchEvaluator

		#endregion

		#region -- class GSubStringMatchEvaluator -----------------------------------------

		///////////////////////////////////////////////////////////////////////////////
		/// <summary></summary>
		private sealed class GSubStringMatchEvaluator : GSubMatchEvaluator
		{
			private object[] replaces;

			public GSubStringMatchEvaluator(string sRepl)
			{
				List<object> lst = new List<object>();
				int i = 0;
				int iStart = 0;

				while (i < sRepl.Length)
				{
					if (sRepl[i] == '%')
					{
						if (++i >= sRepl.Length)
							break;

						if (sRepl[i] == '%') // Parse a number (0-9)
						{
							Add(lst, sRepl, iStart, i);
							iStart = i + 1;
						}
						else if (sRepl[i] >= '0' && sRepl[i] <= '9') // Add what we find until now
						{
							Add(lst, sRepl, iStart, i - 1);
							lst.Add(sRepl[i] - '0');
							iStart = i + 1;
						}
					}

					i++;
				}

				// Add the rest
				Add(lst, sRepl, iStart, i);

				replaces = lst.ToArray();
			} // ctor

			private void Add(List<object> lst, string sRepl, int iStart, int iCurrent)
			{
				int iLength = iCurrent - iStart;
				if (iLength == 0)
					return;

				lst.Add(sRepl.Substring(iStart, iLength));
			} // proc Add

			protected override string MatchEvaluatorImpl(Match m)
			{
				string[] result = new string[replaces.Length];

				for (int i = 0; i < result.Length; i++)
				{
					if (replaces[i] is string)
						result[i] = (string)replaces[i];
					else if (replaces[i] is int)
					{
						int iIndex = (int)replaces[i];
						if (iIndex == 0)
							result[i] = m.Value;
						else if (iIndex <= m.Groups.Count)
							result[i] = m.Groups[iIndex].Value;
						else
							result[i] = String.Empty;
					}
					else
						result[i] = String.Empty;
				}

				return String.Concat(result);
			} // func MatchEvaluator
		} // class GSubLuaTableMatchEvaluator

		#endregion

		#endregion

		public static LuaResult gsub(this string s, string pattern, object repl, int n)
		{
			Regex regex = new Regex(TranslateRegularExpression(pattern));

			if (n <= 0)
				n = Int32.MaxValue;

			GSubMatchEvaluator matchEvaluator;
			if (repl is LuaTable)
				matchEvaluator = new GSubLuaTableMatchEvaluator((LuaTable)repl);
			else if (repl is Delegate || repl is ILuaMethod)
				matchEvaluator = new GSubFunctionMatchEvaluator(repl);
			else
				matchEvaluator = new GSubStringMatchEvaluator((string)Lua.RtConvertValue(repl, typeof(string)));

			string r = regex.Replace(s, matchEvaluator.MatchEvaluator, n);

			return new LuaResult(r, matchEvaluator.MatchCount);
		} // func gsub

		public static int len(this string s)
		{
			return s == null ? 0 : s.Length;
		} // func len

		public static string lower(this string s)
		{
			if (String.IsNullOrEmpty(s))
				return s;
			return s.ToLower();
		} // func lower

		public static LuaResult match(this string s, string pattern, int init = 1)
		{
			if (String.IsNullOrEmpty(s))
				return LuaResult.Empty;
			if (String.IsNullOrEmpty(pattern))
				return LuaResult.Empty;

			// correct the init parameter
			if (init < 0)
				init = s.Length + init + 1;
			if (init <= 0)
				init = 1;

			// translate the regular expression
			pattern = TranslateRegularExpression(pattern);

			Regex r = new Regex(pattern);
			return MatchResult(r.Match(s, init));
		} // func match

		private static LuaResult MatchResult(Match m)
		{
			if (m.Success)
			{
				if (m.Groups.Count > 1) // the expression uses groups, return the groups
				{
					object[] result = new object[m.Groups.Count - 1];

					for (int i = 1; i < m.Groups.Count; i++)
						result[i - 1] = m.Groups[i].Value;

					return result;
				}
				else // no groups, return the captures
				{
					object[] result = new object[m.Captures.Count];

					for (int i = 0; i < m.Captures.Count; i++)
						result[i] = m.Captures[i].Value;

					return result;
				}
			}
			else
				return LuaResult.Empty;
		} // func MatchResult

		public static string rep(this string s, int n, string sep = "")
		{
			if (n == 0)
				return String.Empty;
			return String.Join(sep, Enumerable.Repeat(s, n));
		} // func rep

		public static string reverse(this string s)
		{
			if (String.IsNullOrEmpty(s) || s.Length == 1)
				return s;

			char[] a = s.ToCharArray();
			Array.Reverse(a);
			return new string(a);
		} // func reverse

		public static string sub(this string s, int i, int j = -1)
		{
			if (String.IsNullOrEmpty(s) || j == 0)
				return String.Empty;

			int iStart;
			int iLen;
			NormalizeStringArguments(s, i, j, out iStart, out iLen);

			// return the string
			if (iLen <= 0)
				return String.Empty;
			else
				return s.Substring(iStart, iLen);
		} // func sub

		public static string upper(this string s)
		{
			if (String.IsNullOrEmpty(s))
				return s;
			return s.ToUpper();
		} // func lower

		// todo: pack
		// todo: packsize
		// todo: unpack

		public static bool __TranslateRegEx { get { return lTranslateRegEx; } set { lTranslateRegEx = value; } }
	} // class LuaLibraryString

	#endregion

	#region -- Operating System Facilities ----------------------------------------------

	///////////////////////////////////////////////////////////////////////////////
	/// <summary></summary>
	internal static class LuaLibraryOS
	{
		private static readonly DateTime dtUnixStartTime = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);

		public static LuaResult clock()
		{
			return new LuaResult(Process.GetCurrentProcess().TotalProcessorTime.TotalSeconds);
		} // func clock

		/// <summary>Converts a number representing the date and time back to some higher-level representation.</summary>
		/// <param name="format">Format string. Same format as the C <see href="http://www.cplusplus.com/reference/ctime/strftime/">strftime()</see> function.</param>
		/// <param name="time">Numeric date-time. It defaults to the current date and time.</param>
		/// <returns>Formatted date string, or table of time information.</returns>
		/// <remarks>by PapyRef</remarks>
		public static object date(string format, object time)
		{
			// Unix timestamp is seconds past epoch. Epoch date for time_t is 00:00:00 UTC, January 1, 1970.
			DateTime dt;

			bool lUtc = format != null && format.Length > 0 && format[0] == '!';

			if (time == null)
				dt = lUtc ? DateTime.UtcNow : DateTime.Now;
			else if (time is DateTime)
			{
				dt = (DateTime)time;
				switch (dt.Kind)
				{
					case DateTimeKind.Utc:
						if (!lUtc)
							dt = dt.ToLocalTime();
						break;
					default:
						if (lUtc)
							dt = dt.ToUniversalTime();
						break;
				}
			}
			else
				dt = dtUnixStartTime.AddSeconds((long)Convert.ChangeType(time, typeof(long))).ToLocalTime();

			// Date and time expressed as coordinated universal time (UTC).
			if (lUtc)
				format = format.Substring(1);

			if (String.Compare(format, "*t", false) == 0)
			{
				LuaTable lt = new LuaTable();
				lt["year"] = dt.Year;
				lt["month"] = dt.Month;
				lt["day"] = dt.Day;
				lt["hour"] = dt.Hour;
				lt["min"] = dt.Minute;
				lt["sec"] = dt.Second;
				lt["wday"] = (int)dt.DayOfWeek;
				lt["yday"] = dt.DayOfYear;
				lt["isdst"] = (dt.Kind == DateTimeKind.Local ? true : false);
				return lt;
			}
			else
				return AT.MIN.Tools.ToStrFTime(dt, format);
		} // func date

		/// <summary>Calculate the current date and time, coded as a number. That number is the number of seconds since 
		/// Epoch date, that is 00:00:00 UTC, January 1, 1970. When called with a table, it returns the number representing 
		/// the date and time described by the table.</summary>
		/// <param name="table">Table representing the date and time</param>
		/// <returns>The time in system seconds. </returns>
		/// <remarks>by PapyRef</remarks>
		public static LuaResult time(LuaTable table)
		{
			TimeSpan ts;

			if (table == null)
			{
				// Returns the current time when called without arguments
				ts = DateTime.Now.Subtract(dtUnixStartTime);  //DateTime.UtcNow.Subtract(unixStartTime);
			}
			else
			{
				try
				{
					ts = datetime(table).Subtract(dtUnixStartTime);
				}
				catch (Exception e)
				{
					return new LuaResult(null, e.Message);
				}
			}

			return new LuaResult(Convert.ToInt64(ts.TotalSeconds));
		} // func time

		/// <summary>Converts a time to a .net DateTime</summary>
		/// <param name="time"></param>
		/// <returns></returns>
		public static DateTime datetime(object time)
		{
			if (time is LuaTable)
			{
				LuaTable table = (LuaTable)time;
				return new DateTime(
					table.ContainsKey("year") ? (int)table["year"] < 1970 ? 1970 : (int)table["year"] : 1970,
					table.ContainsKey("month") ? (int)table["month"] : 1,
					table.ContainsKey("day") ? (int)table["day"] : 1,
					table.ContainsKey("hour") ? (int)table["hour"] : 0,
					table.ContainsKey("min") ? (int)table["min"] : 0,
					table.ContainsKey("sec") ? (int)table["sec"] : 0,
					table.ContainsKey("isdst") ? (table.ContainsKey("isdst") == true) ? DateTimeKind.Local : DateTimeKind.Utc : DateTimeKind.Local
				);
			}
			else if (time is int)
				return dtUnixStartTime.AddSeconds((int)time);
			else if (time is double)
				return dtUnixStartTime.AddSeconds((double)time);
			else
				throw new ArgumentException();
		} // func datetime

		/// <summary>Calculate the number of seconds between time t1 to time t2.</summary>
		/// <param name="t2">Higher bound of the time interval whose length is calculated.</param>
		/// <param name="t1">Lower bound of the time interval whose length is calculated. If this describes a time point later than end, the result is negative.</param>
		/// <returns>The number of seconds from time t1 to time t2. In other words, the result is t2 - t1.</returns>
		/// <remarks>by PapyRef</remarks>
		public static long difftime(object t2, object t1)
		{
			long time2 = Convert.ToInt64(t2 is LuaTable ? time((LuaTable)t2)[0] : t2);
			long time1 = Convert.ToInt64(t1 is LuaTable ? time((LuaTable)t1)[0] : t1);

			return time2 - time1;
		} // func difftime

		internal static void SplitCommand(string command, out string sFileName, out string sArguments)
		{
			// check the parameter
			if (command == null)
				throw new ArgumentNullException("command");
			command = command.Trim();
			if (command.Length == 0)
				throw new ArgumentNullException("command");

			// split the command
			if (command[0] == '"')
			{
				int iPos = command.IndexOf('"', 1);
				if (iPos == -1)
				{
					sFileName = command;
					sArguments = null;
				}
				else
				{
					sFileName = command.Substring(1, iPos - 1).Trim();
					sArguments = command.Substring(iPos + 1).Trim();
				}
			}
			else
			{
				sFileName = Path.Combine(Environment.SystemDirectory, "cmd.exe");
				sArguments = "/c " + command;
			}
		} // proc SplitCommand

		public static LuaResult execute(string command, Func<string, LuaResult> output, Func<string, LuaResult> error)
		{
			if (command == null)
				return new LuaResult(true);
			try
			{
				string sFileName;
				string sArguments;
				SplitCommand(command, out sFileName, out sArguments);
				using (Process p = Process.Start(sFileName, sArguments))
				{
					p.WaitForExit();
					return new LuaResult(true, "exit", p.ExitCode);
				}
			}
			catch (Exception e)
			{
				return new LuaResult(null, e.Message);
			}
		} // func execute

		public static void exit(int code = 0, bool close = true)
		{
			Environment.Exit(code);
		} // func exit

		public static string getenv(string varname)
		{
			return Environment.GetEnvironmentVariable(varname);
		} // func getenv

		public static LuaResult remove(string filename)
		{
			try
			{
				File.Delete(filename);
				return new LuaResult(true);
			}
			catch (Exception e)
			{
				return new LuaResult(null, e.Message);
			}
		} // func remove

		public static LuaResult rename(string oldname, string newname)
		{
			try
			{
				File.Move(oldname, newname);
				return new LuaResult(true);
			}
			catch (Exception e)
			{
				return new LuaResult(null, e.Message);
			}
		} // func rename

		public static void setlocale()
		{
			throw new NotImplementedException();
		} // func setlocale

		public static string tmpname()
		{
			return Path.GetTempFileName();
		} // func tmpname
	} // class LuaLibraryOS

	#endregion

	#region -- Debug functions ----------------------------------------------------------

	internal static class LuaLibraryDebug
	{
		public static LuaResult getupvalue(object f, int index)
		{
			return Lua.RtGetUpValue(f as Delegate, index);
		} // func getupvalue

		public static LuaResult upvalueid(object f, int index)
		{
			return new LuaResult(Lua.RtUpValueId(f as Delegate, index));
		} // func upvalueid

		public static LuaResult setupvalue(object f, int index, object v)
		{
			return new LuaResult(Lua.RtSetUpValue(f as Delegate, index, v));
		} // func setupvalue

		public static void upvaluejoin(object f1, int n1, object f2, int n2)
		{
			Lua.RtUpValueJoin(f1 as Delegate, n1, f2 as Delegate, n2);
		} // func upvaluejoin
	} // class LuaLibraryDebug

	#endregion
}
