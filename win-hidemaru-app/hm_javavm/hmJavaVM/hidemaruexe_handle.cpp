#include <windows.h>

#include "hidemaruexe_handle.h"
#include "hidemaruexe_export.h"


HidemaruWindowHandleSearcher::HidemaruWindowHandleSearcher(wstring strClassName) {
	this->m_strClassName = strClassName;
	this->hCurWndHidemaru = NULL;
}

HWND HidemaruWindowHandleSearcher::GetCurWndHidemaru() {
	if (!hCurWndHidemaru) {
		FastSearchCurWndHidemaru(GetDesktopWindow());
	}
	if (!hCurWndHidemaru) {
		SlowSearchCurWndHidemaru(GetDesktopWindow());
	}
	return this->hCurWndHidemaru;
}



bool HidemaruWindowHandleSearcher::IsWndHidemaru32ClassType(HWND hWnd) {
	if ( !hWnd ) {
		return false;
	}

	wchar_t strTargetClassName[MAX_CLASS_NAME];
	::GetClassName(hWnd, strTargetClassName, _countof(strTargetClassName));

	if (m_strClassName == strTargetClassName) {
		return true;
	}
	return false;
}

// ゆっくりだが、秀丸他のアプリの子供になってるなど、超レアケースでも発見出来る。
void HidemaruWindowHandleSearcher::SlowSearchCurWndHidemaru(HWND hWnd)
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

			// 自分と親が両方ともHidemaru32Class(系)なら完全得てい
			if ( hWnd && IsWndHidemaru32ClassType(hWnd) &&
				 hWndParent && IsWndHidemaru32ClassType(hWndParent) ) {
				hCurWndHidemaru = hWnd;
			}
		}
	}

	// 子クラスをなめていく。子クラスはあくまでも「Hidemaru32Class系」。
	// ストア版はちょっと違うのでインスタンス変数になっている。
	HWND hWndChild = FindWindowEx(hWnd, NULL, NULL, NULL);
	while (hWndChild != NULL)
	{
		SlowSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, NULL, NULL);
	}
}

// 速い。通常ならこのアルゴリズムで十分
void HidemaruWindowHandleSearcher::FastSearchCurWndHidemaru(HWND hWnd)
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
			if (hWndParent && IsWndHidemaru32ClassType(hWndParent))
			{
				hCurWndHidemaru = hWnd;
			}
		}
	}

	// 子クラスをなめていく。子クラスはあくまでも「Hidemaru32Class系」。
	// ストア版はちょっと違うのでインスタンス変数になっている。
	HWND hWndChild = FindWindowEx(hWnd, NULL, m_strClassName.c_str(), NULL);
	while (hWndChild != NULL)
	{
		FastSearchCurWndHidemaru(hWndChild);
		if (hCurWndHidemaru) { break; }

		hWndChild = FindWindowEx(hWnd, hWndChild, m_strClassName.c_str(), NULL);
	}
}

static HWND hWnd = NULL;
HWND GetCurWndHidemaru() {
	if (!hWnd) {
		// 普通の秀丸
		HidemaruWindowHandleSearcher s1(L"Hidemaru32Class");
		hWnd = s1.GetCurWndHidemaru();
	}
	if (!hWnd) {
		// ストアアプリ版
		HidemaruWindowHandleSearcher s2(L"Hidemaru32Class_Appx");
		hWnd = s2.GetCurWndHidemaru();
	}

	return hWnd;
}
