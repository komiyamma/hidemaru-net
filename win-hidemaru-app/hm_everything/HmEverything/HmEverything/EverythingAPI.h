#pragma once

#include "Everything.h"

#include <string>
#include <msclr/marshal_cppstd.h>

using namespace std;

using namespace msclr::interop;
using namespace System;
using namespace System::Text;

namespace HmEverything {
	public ref class Everything {
	public:
		Everything() {

		}
	public:
		static void Everything_SetSearchW(String^ lpSearchString) {
			wstring s = marshal_as<wstring>(lpSearchString);
			::Everything_SetSearchW(s.data());
		}
		static void Everything_SetMatchPath(bool bEnable) {
			::Everything_SetMatchPath(bEnable);
		}
		static void Everything_SetMatchCase(bool bEnable) {
			::Everything_SetMatchCase(bEnable);
		}
		static void Everything_SetMatchWholeWord(bool bEnable) {
			::Everything_SetMatchWholeWord(bEnable);
		}
		static void Everything_SetRegex(bool bEnable) {
			::Everything_SetRegex(bEnable);
		}
		static void Everything_SetMax(int dwMax) {
			::Everything_SetMax(dwMax);
		}
		static void Everything_SetOffset(int dwOffset) {
			::Everything_SetOffset(dwOffset);
		}
		static void Everything_SetReplyWindow(IntPtr hWnd) {
			HWND h = (HWND)hWnd.ToPointer();
			::Everything_SetReplyWindow(h);
		}
		static void Everything_SetReplyID(int nId) {
			::Everything_SetReplyID(nId);
		}
		static bool Everything_GetMatchPath() {
			return ::Everything_GetMatchPath();
		}
		static bool Everything_GetMatchCase() {
			return ::Everything_GetMatchCase();
		}
		static bool Everything_GetMatchWholeWord() {
			return ::Everything_GetMatchWholeWord();
		}
		static bool Everything_GetRegex() {
			return ::Everything_GetRegex();
		}
		static UInt32 Everything_GetMax() {
			return ::Everything_GetMax();
		}
		static UInt32 Everything_GetOffset() {
			return ::Everything_GetOffset();
		}
		static String^ Everything_GetSearch() {
			const wchar_t *s = ::Everything_GetSearch();
			return gcnew String(s);
		}
		static int Everything_GetLastError() {
			return ::Everything_GetLastError();
		}
		static IntPtr Everything_GetReplyWindow() {
			HWND h = ::Everything_GetReplyWindow();
			return IntPtr(h);
		}
		static int Everything_GetReplyID() {
			return ::Everything_GetReplyID();
		}
		static bool Everything_QueryW(bool bWait) {
			return ::Everything_QueryW(bWait);
		}
		static bool Everything_IsQueryReply(int message, IntPtr wParam, IntPtr lParam, UInt32 nId) {
			WPARAM wP = (WPARAM)wParam.ToPointer();
			LPARAM lP = (LPARAM)lParam.ToPointer();
			return ::Everything_IsQueryReply(message, wP, lP, nId);
		}
		static void Everything_SortResultsByPath() {
			return ::Everything_SortResultsByPath();
		}
		static int Everything_GetNumFileResults() {
			return ::Everything_GetNumFileResults();
		}
		static int Everything_GetNumFolderResults() {
			return ::Everything_GetNumFolderResults();
		}
		static int Everything_GetNumResults() {
			return ::Everything_GetNumResults();
		}
		static int Everything_GetTotFileResults() {
			return ::Everything_GetTotFileResults();
		}
		static int Everything_GetTotFolderResults() {
			return ::Everything_GetTotFolderResults();
		}
		static int Everything_GetTotResults() {
			return ::Everything_GetTotResults();
		}
		static bool Everything_IsVolumeResult(int nIndex) {
			return ::Everything_IsVolumeResult(nIndex);
		}
		static bool Everything_IsFolderResult(int nIndex) {
			return ::Everything_IsFolderResult(nIndex);
		}
		static bool Everything_IsFileResult(int nIndex) {
			return ::Everything_IsFileResult(nIndex);
		}
		static String^ Everything_GetResultFullPathNameW(int nIndex) {
			wchar_t buf[1024];
			DWORD r = ::Everything_GetResultFullPathNameW(nIndex, buf, _countof(buf));
			String^ strFullPathName = gcnew String(buf);
			return strFullPathName;
		}
		static void Everything_Reset() {
			return ::Everything_Reset();
		}
	};
}