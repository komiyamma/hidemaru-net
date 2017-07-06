#include <stdio.h>
#include <windows.h>
#include <string>
#include <msclr/marshal_cppstd.h>

#include "Everything.h"

#include "convert_string.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"
#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"
#include "output_debugstream.h"


#include "EverythingSearchForm.h"

using namespace HmEverything;

#pragma data_seg("HM_SHARED_EVERYTING_SEG")
HWND hSharedHidemaruWndHandle = 0;
#pragma data_seg()

MACRO_DLL HWND GetSharedWndHidemaru() {
	HWND hWNd = hSharedHidemaruWndHandle;
	if (IsWindow(hWNd)) {
		return hSharedHidemaruWndHandle;
	}
	else {
		hSharedHidemaruWndHandle = NULL;
		return NULL;
	}
}

MACRO_DLL intHM_t Show(HWND hWnd, int iOptionalFlags) {
	hSharedHidemaruWndHandle = hWnd;

	// DoStringされる度にdllのBindの在り方を確認更新する。
	CSelfDllInfo::SetBindDllHandle();

	EverythingSearchForm::form = gcnew EverythingSearchForm(hWnd, iOptionalFlags);

	// フォームを表示。
	EverythingSearchForm::form->ShowDialog();

	return TRUE;
}


MACRO_DLL intHM_t DestroyScope() {
	try {
		// まだ残っていたら(フォームを手動で閉じていたら、残っていない)
		if (EverythingSearchForm::form != nullptr) {


			//監視を終了
			EverythingSearchForm::form->Stop();

			// 閉じてNull
			EverythingSearchForm::form = nullptr;

			// 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
			GC::Collect();
			GC::WaitForPendingFinalizers();
		}
	}
	catch (Exception^) {}

	return TRUE;

	TestDynamicVar.Clear();

	return TRUE;
}


MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	return DestroyScope();
}