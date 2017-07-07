#pragma once

#include "hidemaruexe_export.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

using namespace OpenQA::Selenium;
using namespace OpenQA::Selenium::Chrome;

CHidemaruExeExport HMEXE;


/// やたらめったらtry_catchなのは、「いつ不意に秀丸本体が閉じても可能な限り黙って終了できるようにするため」
enum class hmHtmlPreviewMode { Live = 0, File = 1 };


/// <summary> モードに関わらず共通 </summary>
public ref class HmHtmlPreviewChromeForm : System::Windows::Forms::Form
{
public:
	static HmHtmlPreviewChromeForm^ form;
protected:
	HWND hWndHidemaru = NULL;
	String^ strCurFileFullPath = "";
	String^ strPrvFileFullPath = "";
	String^ strPrvHmEditTotalText = "";
	hmHtmlPreviewMode mode;
	Timer^ update;

protected:
	IWebDriver^ driver;
	ChromeOptions^ chromeOptions;
	ChromeDriverService^ chromeService;

public:

	HmHtmlPreviewChromeForm(HWND hWndHidemaru)
	{
		this->hWndHidemaru = hWndHidemaru;

		SetFormAttribute();
		SetWebBrowserAttribulte();
		SetTimerAttribute();
	}

protected:
	/// <summary>現在の秀丸プロセスでの秀丸が編集しているファイル名を取得する(異なるプロセスの秀丸は関係ない)</summary>
	String^ GetCurrentHidemaruFileName() {
		strPrvFileFullPath = strCurFileFullPath;
		const int WM_HIDEMARUINFO = WM_USER + 181;
		const int HIDEMARUINFO_GETFILEFULLPATH = 4;

		wchar_t szCurrentFileFullPath[MAX_PATH * 2] = L""; // 秀丸のSetHidemaruHandleが呼ばれた時に最初にスロットに入るファイル名。途中でファイル名が変わることがあるので、あくまでもなかった場合用途

		if (!IsWindow(hWndHidemaru)) {
			hWndHidemaru = CHidemaruExeExport::GetCurWndHidemaru();   // 現在の秀丸ウィンドウのファイル名を得る。
		}
		// 現在の秀丸ウィンドウのファイル名を得る。
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileFullPath);
		if (cwch > 0) {
			return gcnew String(szCurrentFileFullPath);
		}
		else {
			return L"";
		}
	}

	/// <summary>フォームの属性設定</summary>
	void SetFormAttribute() {

		// 効果なさそうだが、一応Waitカーソルは使わない
		Application::UseWaitCursor = false;
	}

	/// <summary>Webブラウザ属性設定</summary>
	void SetWebBrowserAttribulte() {
		try {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			// 秀丸で編集中のテキストが変わったのだよ。
			if (strCurFileFullPath->Length > 0) {

				if (strPrvFileFullPath != strCurFileFullPath) {
					strPrvFileFullPath = strCurFileFullPath;
					SetWatcherAttribute();
				}
			}
		}
		catch (Exception^ e) {
			System::Diagnostics::Debug::WriteLine(e->Message);
		}

		try {
			chromeOptions = gcnew ChromeOptions();
			String ^self_full_path = System::Reflection::Assembly::GetExecutingAssembly()->Location;
			String ^self_dir = System::IO::Path::GetDirectoryName(self_full_path);

			chromeService = ChromeDriverService::CreateDefaultService(self_dir);
			chromeService->HideCommandPromptWindow = true;

			driver = gcnew ChromeDriver(chromeService, chromeOptions);
			watcher_Renamed(strCurFileFullPath);
		}
		catch (Exception^ e) {
			System::Diagnostics::Debug::WriteLine(e->Message);
		}
	}

	void SetTimerAttribute() {
		// １秒に１回の更新で十分だろう。
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmHtmlPreviewChromeForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	void update_Tick(Object ^sender, EventArgs ^e) {
		try  {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			if (strCurFileFullPath->Length > 0) {
				if (strPrvFileFullPath != strCurFileFullPath) {
					strPrvFileFullPath = strCurFileFullPath;
					SetWatcherAttribute();
				}
			}
			else {
				Stop();
			}
		}
		catch (Exception^) {}
	}

public:
	virtual void Stop() {
		try {
			if (update != nullptr) {
				// フォームを閉じた時も解放。
				update->Stop();
				update = nullptr;
			}
			// watcherを特別に追加してるのでそれも止める
			if (watcher != nullptr) {
				//監視を終了
				watcher->EnableRaisingEvents = false;
				watcher = nullptr;
			}
		}
		catch (Exception^) {}
	}

private:

	/// <summary>ファイルの監視</summary>
	IO::FileSystemWatcher^ watcher = nullptr;

	bool isNavigateChange = false;
	/// <summary>ファイルの監視の属性。
	/// 秀丸がファイル名を変更したり、ディレクトリを変更したりしても追跡できるようにするため。
	/// 又、ファイルの保存時は、webBrowserの内容を更新する。
	/// </summary>
	void SetWatcherAttribute() {
		try {
			if (strCurFileFullPath->Length > 0) {
				if (watcher == nullptr) {
					watcher = gcnew IO::FileSystemWatcher();
					watcher->NotifyFilter =
						(System::IO::NotifyFilters::LastAccess
						| System::IO::NotifyFilters::LastWrite
						| System::IO::NotifyFilters::FileName
						| System::IO::NotifyFilters::DirectoryName);
					;
				}

				watcher->Path = System::IO::Path::GetDirectoryName(strCurFileFullPath);
				// wbとは内容更新が非同期衝突するので、そこは同期せよ
				watcher->Filter = "*.*";

				watcher->Changed += gcnew FileSystemEventHandler(this, &HmHtmlPreviewChromeForm::watcher_Changed);
				watcher->IncludeSubdirectories = false;

				//監視を開始する
				watcher->EnableRaisingEvents = true;

				watcher_Renamed(strCurFileFullPath);
			}
		}
		catch (Exception^) {
		}
	}

	/// <summary>ファイルの監視の属性。
	/// 秀丸がファイル名を変更したり、ディレクトリを変更したりしても追跡できるようにするため。
	/// 又、ファイルの保存時は、webBrowserの内容を更新する。
	/// </summary>
	void watcher_Changed(Object^ o, FileSystemEventArgs^ e) {
		try {
			System::Diagnostics::Trace::WriteLine(e->ChangeType);
			System::Diagnostics::Trace::WriteLine("\n");

			if (e->ChangeType == IO::WatcherChangeTypes::Changed) {
				// 秀丸で編集中のテキストが変わったのだよ。
				if (String::Compare(e->FullPath, strCurFileFullPath, true)) {
					driver->Navigate()->Refresh();
				}
			}
		}
		catch (Exception^) {
		}
	}

	void watcher_Renamed(String^ strCurFileFullPath) {
		// ここを通過するということはファイルのフルパスが変更となったのであろうから、Urlの更新
		driver->Url = strCurFileFullPath;
	}

};

