#include <stdio.h>
#include <windows.h>
#include <string>
#include <msclr/marshal_cppstd.h>

#include "convert_string.h"
#include "hidemaruexe_export.h"
#include "self_dll_info.h"
#include "dllfunc_interface.h"
#include "dllfunc_interface_internal.h"
#include "output_debugstream.h"


#include "RipGrepSearchForm.h"

using namespace HmRipGrep;

#pragma data_seg("HM_SHARED_REPGREP_SEG")
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

	RipGrepSearchForm::form = gcnew RipGrepSearchForm(hWnd, iOptionalFlags);

	// フォームを表示。
	RipGrepSearchForm::form->ShowDialog();

	return TRUE;
}


MACRO_DLL intHM_t DestroyScope() {
	try {
		// まだ残っていたら(フォームを手動で閉じていたら、残っていない)
		if (RipGrepSearchForm::form != nullptr) {


			//監視を終了
			RipGrepSearchForm::form->Stop();

			// 閉じてNull
			RipGrepSearchForm::form = nullptr;

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