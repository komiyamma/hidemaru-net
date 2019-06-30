#include <windows.h>

#include "hidemaruexe_handle.h"
#include "string_converter.h"

HidemaruWindowHandleSearcher::HidemaruWindowHandleSearcher(String^ strClassName) {
	this->strHidemaruClassName = strClassName;
	this->hCurWndHidemaru = NULL;
}

HWND HidemaruWindowHandleSearcher::SearchCurWndHidemaru() {
	// まず普通のタブモードの秀丸
	if (!hCurWndHidemaru) {
		TabOnSearchCurWndHidemaru(GetDesktopWindow());
	}
	// 次にタブではないモードの秀丸
	if (!hCurWndHidemaru) {
		NoTabSearchCurWndHidemaru(GetDesktopWindow());
	}

	// 以上の２つの順番で検索していく必要がある
	return this->hCurWndHidemaru;
}



bool HidemaruWindowHandleSearcher::IsWndHidemaru32ClassType(HWND hWnd) {
	if (!hWnd) {
		return false;
	}

	wchar_t strTargetClassName[MAX_CLASS_NAME];
	::GetClassName(hWnd, strTargetClassName, _countof(strTargetClassName));

	if (strHidemaruClassName == gcnew String(strTargetClassName)) {
		return true;
	}
	return false;
}

// ゆっくりだが、秀丸他のアプリの子供になってるなど、超レアケースでも発見出来る。
void HidemaruWindowHandleSearcher::TabOnSearchCurWndHidemaru(HWND hWnd)
{
	if (hCurWndHidemaru) { return; }

	HWND hWndParent = ::GetParent(hWnd);

	// 親があることが条件
	if (hWndParent) {
		// 自分のプロセスIDと、サーチ対象のプロセスID
		DWORD pID1 = GetCurrentProcessId();
		DWORD pID2 = 0;
		GetWindowThreadProcessId(hWnd, &pID2);
		// 同じなら大きく候補だ
		if (pID1 == pID2) {

			// 自分自身の親も指定のクラス名なら、完全に特定した。
			if (IsWndHidemaru32ClassType(hWndParent))
			{
				hCurWndHidemaru = hWnd;
			}
		}
	}

	// 子クラスをなめていく。子クラスはあくまでも「Hidemaru32Class系」。
	// ストア版はちょっと違うのでインスタンス変数になっている。
	const wchar_t *pszClassName = String_to_wstring(strHidemaruClassName).c_str();
	HWND hWndChild = FindWindowEx(hWnd, NULL, pszClassName, NULL);
	while (hWndChild != NULL)
	{
		TabOnSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, pszClassName, NULL);
	}
}

// 速い。通常ならこのアルゴリズムで十分
void HidemaruWindowHandleSearcher::NoTabSearchCurWndHidemaru(HWND hWnd)
{
	if (hCurWndHidemaru) { return; }

	HWND hWndParent = ::GetParent(hWnd);

	if (IsWndHidemaru32ClassType(hWnd)) {
		// 自分のプロセスIDと、サーチ対象のプロセスID
		DWORD pID1 = GetCurrentProcessId();
		DWORD pID2 = 0;
		GetWindowThreadProcessId(hWnd, &pID2);
		// 同じなら大きく候補だ
		if (pID1 == pID2) {

			hCurWndHidemaru = hWnd;
		}
	}

	// 子クラスをなめていく。子クラスはあくまでも「Hidemaru32Class系」。
	// ストア版はちょっと違うのでインスタンス変数になっている。
	const wchar_t *pszClassName = String_to_wstring(strHidemaruClassName).c_str();
	HWND hWndChild = FindWindowEx(hWnd, NULL, pszClassName, NULL);
	while (hWndChild != NULL)
	{
		NoTabSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, pszClassName, NULL);
	}
}

IntPtr HidemaruWindowHandleSearcher::GetCurWndHidemaru(IntPtr _hCurWnd) {
	HWND hCurWnd = (HWND)_hCurWnd.ToPointer();
	// 現状が機能しているならそのまま
	if (IsWindow(hCurWnd)) {
		return (IntPtr)hCurWnd;
	}

	// 元々はいっていなかったら、もしくは、タブモードの切り替え等で機能しなくなった

	HWND hWnd = NULL;
	if (!hWnd) {
		HidemaruWindowHandleSearcher s1(L"Hidemaru32Class");
		hWnd = s1.SearchCurWndHidemaru();
	}

	if (!hWnd) {
		// ストアアプリ版
		HidemaruWindowHandleSearcher s2(L"Hidemaru32Class_Appx");
		hWnd = s2.SearchCurWndHidemaru();
	}

	return (IntPtr)hWnd;
}

