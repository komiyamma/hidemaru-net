#pragma once

#include "hidemaruexe_export.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

CHidemaruExeExport HMEXE;

#pragma comment(lib, "urlmon")

/// やたらめったらtry_catchなのは、「いつ不意に秀丸本体が閉じても可能な限り黙って終了できるようにするため」
enum class hmHtmlPreviewMode { Live = 0, File = 1 };


/// <summary> モードに関わらず共通 </summary>
public ref class HmHtmlBaseForm abstract : System::Windows::Forms::Form
{
public:
	static HmHtmlBaseForm^ form;
protected:
	HWND hWndHidemaru = NULL;
	String^ strCurFileFullPath = "";
	String^ strPrvFileFullPath = "";
	String^ strPrvHmEditTotalText = "";
	hmHtmlPreviewMode mode;
	Timer^ update;
	WebBrowser^ wb;
public:

	HmHtmlBaseForm(HWND hWndHidemaru)
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
		this->Text = "秀丸用 HmHtmlPreview";

		// アセンブリ取得～アイコン設定
		System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.Resource", prj_assebmly->GetName()->Name), prj_assebmly);
		System::Drawing::Icon^ icon = (System::Drawing::Icon^)(r->GetObject("ie"));
		this->Icon = icon;

		// このフォームサイズ
		this->Width = 400;
		this->Height = 600;

		// 効果なさそうだが、一応Waitカーソルは使わない
		Application::UseWaitCursor = false;

		// 閉じる直前
		this->FormClosing += gcnew FormClosingEventHandler(this, &HmHtmlBaseForm::form_FormClosing);
	}

	/// <summary>Webブラウザ属性設定</summary>
	void SetWebBrowserAttribulte() {
		// WebBrowserオブジェクトを配置
		wb = gcnew WebBrowser();
		wb->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);
		wb->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler(this, &HmHtmlBaseForm::wb_DocumentCompleted);
		wb->ClientSize = this->ClientSize;

		// スクリプトエラーでもダイアログとか出さない
		wb->ScriptErrorsSuppressed = true;

		// フォームのサイズ変化した時追従
		this->SizeChanged += gcnew EventHandler(this, &HmHtmlBaseForm::form_SizeChanged);
		this->Controls->Add(wb);

		wb_DocumentInit();
	}

	/// <summary>タイマー属性設定。データ更新の必要性があるかどうかの基礎更新。
	/// Liveモードでは、このタイミングがデータ更新のタイミング。
	/// Fileモードでは、このタイミングはユーザーが「なんのファイル」を開いているのか、の情報の更新のみ。
	///</summary>
	void SetTimerAttribute() {
		// １秒に１回の更新で十分だろう。
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmHtmlBaseForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	void update_Tick(Object ^sender, EventArgs ^e) {
		update_Tick_Implements(sender, e);
	}

	virtual void update_Tick_Implements(Object ^sender, EventArgs ^e) = 0;

	//=========================================================================================
	// フォーム
	//=========================================================================================
	void form_SizeChanged(Object^ o, EventArgs^ e) {
		try {
			// WebBrowserをフォーム全体に。
			wb->ClientSize = this->ClientSize;
		}
		catch (Exception^) {}
	}

	void form_FormClosing(Object^ o, FormClosingEventArgs^ e) {
		try {
			//監視を終了
			Stop();

			// ガベージコレクトもしておく。
			GC::Collect();
		}
		catch (Exception^) {}
	}

	// wbはドキュメントが更新されると、ページ位置が強制的にページトップへと移動してしまう。
	// よってドキュメントを更新する前に、座標を保存しておき、ドキュメント更新後に、元へと戻すため
	Point webBrowserScroll = Point(0, 0);

	// 文章内容が更新した、ということを手動で。
	bool isDocumentChanged = false;


	virtual void wb_DocumentInit() = 0;

public:
	virtual void Stop() {
		try {
			if (update != nullptr) {
				// フォームを閉じた時も解放。
				update->Stop();
				update = nullptr;
			}
		}
		catch (Exception^) {}
	}

protected:
	/// <summary> wbのドキュメントの更新が完全に完了した時(最初の読み込み時も完了するとここに来る) </summary>
	void wb_DocumentCompleted(Object^ sender, WebBrowserDocumentCompletedEventArgs^ e)
	{
		try {
			if (isDocumentChanged) {
				isDocumentChanged = false;
				// 保持していた座標へとスクロール
				wb->Document->Window->ScrollTo(webBrowserScroll.X, webBrowserScroll.Y);
				// 音を戻す
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);
			}
		}
		catch (Exception^) {}
	}
};

