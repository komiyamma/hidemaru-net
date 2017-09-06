#include <map>
#include <fstream>
#include <sstream>
#include <shlwapi.h>
#include <regex>
#include <locale.h>
#include <tchar.h>

#include "dllmain.h"


using namespace std;

using intHM_t = intptr_t;

struct KMJ_DataType {
	intHM_t ID;
	intHM_t iPosX;
	intHM_t iPosY;
	intHM_t hWndHide;
	wstring szFileNameFullPath;
	intHM_t iSplitMode;
};

static map<intHM_t, KMJ_DataType> kmj_HashMap;

wstring KMJ_GetSelfTxtFullPath() {
	wstring dllname = GetSelfDllFullPath();
	TCHAR szDllName[MAX_PATH] = _T("");
	wcscpy_s(szDllName, dllname.data());
	BOOL s = PathRenameExtension(szDllName, _T(".txt"));
	if (s) {
		return wstring(szDllName);
	}
	else {
		return dllname + _T(".txt");
	}
}

static bool isReadedMarkerSaveFile = false;

static bool KMJ_ReadMarkerSaveFile() {
	wstring szFileName = KMJ_GetSelfTxtFullPath();
	if (!PathFileExists(szFileName.data())) {
		FILE* dummy = _wfopen(szFileName.data(), _T("wt, ccs=UNICODE"));
		ofstream ofs(dummy);

		if (ofs.fail()) {
			return false;
		}

		// 空ファイルを作るのみ
		ofs.close();
	}

	FILE* file = _wfopen(szFileName.data(), _T("rt, ccs=UNICODE"));
	wifstream ifs(file);

	if (ifs.fail()) {
		return false;
	}

	wstring bufline;

	while (ifs && getline(ifs, bufline)) {
		// ID,X座標,Y座標,フルファイルパス名
		const wregex wre(_T("^([^,]+?),([^,]+?),([^,]+?),([^,]+),([^,]+),([^,]+)$"));
		std::wsmatch wmt;
		if (std::regex_search(bufline, wmt, wre)) {
			// ID
			wstring strID = wmt[1];
			intHM_t iID = 0;
			try { iID = (intHM_t)std::stoll(strID); }
			catch (...) {}

			// 座標X
			wstring strPosX = wmt[2];
			intHM_t iPosX = 0;
			try { iPosX = (intHM_t)std::stoll(strPosX); }
			catch (...) {}

			// 座標X
			wstring strPosY = wmt[3];
			intHM_t iPosY = 0;
			try { iPosY = (intHM_t)std::stoll(strPosY); }
			catch (...) {}

			// 座標X
			wstring hStrWndHandle = wmt[4];
			intHM_t iWndHandle = 0;
			try { iWndHandle = (intHM_t)std::stoll(hStrWndHandle); }
			catch (...) {}


			// ファイル名
			wstring strFileName = wmt[5];

			// 分割なし、上下、左右
			wstring strSplitMode = wmt[6];
			intHM_t iSplitMode = 0;
			try { iSplitMode = (intHM_t)std::stol(strSplitMode); }
			catch (...) {}

			// 必須なデータは入っている
			if (strID.length() > 0 && strFileName.length() > 0) {
				// ファイルは存在している
				KMJ_DataType data = { iID, iPosX, iPosY, iWndHandle, strFileName, iSplitMode };
				kmj_HashMap[iID] = data;
			}
		}
	}

	ifs.close();

	isReadedMarkerSaveFile = true;
	return true;
}

static bool KMJ_WriteMarkerSaveFile(KMJ_DataType curdata) {
	FILE* file = _wfopen(KMJ_GetSelfTxtFullPath().data(), _T("wt, ccs=UNICODE"));
	wofstream ofs(file);

	if (ofs.fail()) {
		return false;
	}

	for (auto elem : kmj_HashMap) {
		auto data = elem.second;
		wstringstream ss;
		ss << data.ID << "," << data.iPosX << "," << data.iPosY << "," << data.hWndHide << "," << data.szFileNameFullPath << "," << data.iSplitMode;
		wstring line = ss.str();
		ofs << line << endl;
	}

	ofs.close();

	return true;
}

intHM_t __cdecl KMJ_StockMarker(intHM_t iInputKeyID, intHM_t iPosX, intHM_t iPosY, intHM_t hWndHide, const TCHAR *szFileFullPath, intHM_t iSplitMode) {

	// もし
	if (KMJ_ReadMarkerSaveFile()) {

		// 今回のデータを追加して…
		KMJ_DataType curdata = { iInputKeyID, iPosX, iPosY, hWndHide, wstring(szFileFullPath), iSplitMode };
		kmj_HashMap[curdata.ID] = curdata;

		KMJ_WriteMarkerSaveFile(curdata);
	}

	return TRUE;
}

intHM_t __cdecl KMJ_GetPosX(intHM_t iInputKeyID) {
	if (isReadedMarkerSaveFile || KMJ_ReadMarkerSaveFile()) {
		if (kmj_HashMap[iInputKeyID].ID == iInputKeyID) {
			return kmj_HashMap[iInputKeyID].iPosX;
		}
	}
	return 0;
}

intHM_t __cdecl KMJ_GetPosY(intHM_t iInputKeyID) {
	if (isReadedMarkerSaveFile || KMJ_ReadMarkerSaveFile()) {
		if (kmj_HashMap[iInputKeyID].ID == iInputKeyID) {
			return kmj_HashMap[iInputKeyID].iPosY;
		}
	}
	return 0;
}

intHM_t __cdecl KMJ_GetWindowHandle(intHM_t iInputKeyID) {
	if (isReadedMarkerSaveFile || KMJ_ReadMarkerSaveFile()) {
		if (kmj_HashMap[iInputKeyID].ID == iInputKeyID) {
			TCHAR szClassName[1024];
			GetClassName((HWND)kmj_HashMap[iInputKeyID].hWndHide, szClassName, _countof(szClassName));    // クラス文字列の取得
			// 秀丸の有効なウィンドウハンドルである。
			if (wcsstr(szClassName, _T("maru"))) {
				return kmj_HashMap[iInputKeyID].hWndHide;
			}
			NULL;
		}
	}
	return NULL;
}

const TCHAR* __cdecl KMJ_GetFileFullPath(intHM_t iInputKeyID) {
	if (isReadedMarkerSaveFile || KMJ_ReadMarkerSaveFile()) {
		if (kmj_HashMap[iInputKeyID].ID == iInputKeyID) {
			return kmj_HashMap[iInputKeyID].szFileNameFullPath.data();
		}
	}
	return NULL;
}

intHM_t __cdecl KMJ_GetSplitMode(intHM_t iInputKeyID) {
	if (isReadedMarkerSaveFile || KMJ_ReadMarkerSaveFile()) {
		if (kmj_HashMap[iInputKeyID].ID == iInputKeyID) {
			return kmj_HashMap[iInputKeyID].iSplitMode;
		}
	}
	return NULL;
}

