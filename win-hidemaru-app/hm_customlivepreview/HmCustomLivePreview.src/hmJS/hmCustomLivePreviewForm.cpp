/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include <windows.h>

#include "hidemaruexe_export.h"

#include "hmCustomLivePreview.h"
#include "hmCustomLivePreviewStatlcLib.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

#pragma comment(lib, "urlmon")

public ref class HmCustomLivePreviewForm : System::Windows::Forms::Form
{
public:
	static HmCustomLivePreviewForm^ form;
private:
	HWND hWndHidemaru = NULL;
	String^ strCurFileFullPath = "";
	String^ strPrvFileFullPath = "";
	String^ strPrvHmEditTotalText = "";
public:
	SplitContainer^ sc;

	Timer^ update;
	WebBrowser^ wb;
	RichTextBox^ tb;

public:


	HmCustomLivePreviewForm(HWND hWndHidemaru)
	{
		this->hWndHidemaru = hWndHidemaru;

		SetFormAttribute();
		SetTextBoxAttribute();
		SetWebBrowserAttribulte();
		SetTimerAttribute();
	}

	void SetSplitContainer() {
		sc = gcnew SplitContainer();
		sc->BorderStyle = BorderStyle::FixedSingle;
		this->Controls->Add(sc);
		sc->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);

		sc->SplitterMoved += gcnew SplitterEventHandler(this, &HmCustomLivePreviewForm::sc_SplitterMoved);
	}

	String^ GetCurrentHidemaruFileName() {
		strPrvFileFullPath = strCurFileFullPath;
		const int WM_HIDEMARUINFO = WM_USER + 181;
		const int HIDEMARUINFO_GETFILEFULLPATH = 4;

		wchar_t szCurrentFileFullPath[MAX_PATH * 2] = L""; // 秀丸のSetHidemaruHandleが呼ばれた時に最初にスロットに入るファイル名。途中でファイル名が変わることがあるので、あくまでもなかった場合用途

		if (!IsWindow(hWndHidemaru)) {
			hWndHidemaru = CHidemaruExeExport::GetCurWndHidemaru();   // 現在の秀丸ウィンドウのファイル名を得る。
		}
		LRESULT cwch = SendMessage(hWndHidemaru, WM_HIDEMARUINFO, HIDEMARUINFO_GETFILEFULLPATH, (LPARAM)szCurrentFileFullPath);
		if (cwch > 0) {
			return gcnew String(szCurrentFileFullPath);
		}
		else {
			return L"";
		}
	}

	void SetFormAttribute() {

		SetSplitContainer();

		this->Text = "秀丸用 HmCustomLivePreview";

		// アセンブリ取得～アイコン設定
		// System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		// System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.Resource", prj_assebmly->GetName()->Name), prj_assebmly);
		// System::Drawing::Icon^ icon = (System::Drawing::Icon^)(r->GetObject("md"));
		// this->Icon = icon;

		// このフォームサイズ
		this->Width = 600;
		this->Height = 600;
		sc->ClientSize = this->ClientSize;

		// 効果なさそうだが、一応Waitカーソルは使わない
		Application::UseWaitCursor = false;

		// 閉じる直前
		this->FormClosing += gcnew FormClosingEventHandler(this, &HmCustomLivePreviewForm::form_FormClosing);
	}

	void SetTextBoxAttribute() {
		tb = gcnew RichTextBox();
		tb->Multiline = true;
		tb->ScrollBars = ::RichTextBoxScrollBars::Vertical;
		sc->Panel1->Controls->Add(tb);
		tb->ClientSize = sc->Panel1->ClientSize;
	}

	void SetWebBrowserAttribulte() {
		// WebBrowserオブジェクトを配置
		wb = gcnew WebBrowser();
		wb->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);
		wb->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler(this, &HmCustomLivePreviewForm::wb_DocumentCompleted);

		// 右側に配置。目いっぱい
		wb->ClientSize = sc->Panel2->ClientSize;

		// スクリプトエラーでもダイアログとか出さない
		wb->ScriptErrorsSuppressed = true;

		// フォームのサイズ変化した時追従
		this->SizeChanged += gcnew EventHandler(this, &HmCustomLivePreviewForm::form_SizeChanged);
		sc->Panel2->Controls->Add(wb);

	}

	void SetTimerAttribute() {
		// １秒に１回の更新で十分だろう。
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmCustomLivePreviewForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	//=========================================================================================
	// フォームロード
	//=========================================================================================
private:

	void form_SizeChanged(Object^ o, EventArgs^ e) {
		try {
			sc->ClientSize = this->ClientSize;

			// 左側のサイズ調整
			tb->ClientSize = sc->Panel1->ClientSize;

			// WebBrowserを右側全体に。
			wb->ClientSize = sc->Panel2->ClientSize;

		}
		catch (Exception^) {}
	}

	void sc_SplitterMoved(Object^ o, SplitterEventArgs^ e) {
		form_SizeChanged(nullptr, nullptr);
	}

	void form_FormClosing(Object^ o, FormClosingEventArgs^ e) {
		try {
			// フォームを閉じた時も解放。
			update->Stop();
			update = nullptr;

			// 音を戻す
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			// インタプリタの分の破棄
			(intHM_t)ICustomLivePreviewStaticLib::DestroyScope();

			// ガベージコレクトもしておく。
			GC::Collect();
		}
		catch (Exception^) {}
	}

	Point webBrowserScroll = Point(0, 0);
	bool isDocumentChanged = false;
	//=========================================================================================
	// タイマーイベント
	//=========================================================================================
	void update_Tick(Object ^sender, EventArgs ^e)
	{
		try {

			String^ curHmHTMLTotalText = ICustomLivePreviewStaticLib::OnCustomTranslateHTML(strCurFileFullPath);

			strCurFileFullPath = GetCurrentHidemaruFileName();

			// テキストが変化した時、もしくは、ファイル名が変化した時に更新
			if (strPrvHmEditTotalText != curHmHTMLTotalText || strCurFileFullPath != strPrvFileFullPath) {

				strPrvHmEditTotalText = curHmHTMLTotalText;

				if (wb->Document != nullptr)
				{
					// 手段その①．
					// Document->Bodyが取れるパターン。これでは失敗するときもある。
					webBrowserScroll.X = wb->Document->Body->ScrollLeft;
					webBrowserScroll.Y = wb->Document->Body->ScrollTop;

					// 手段その②．
					// HTMLエレメントのScroll位置を見に行くパターン。こちらも失敗するときもある。
					if (webBrowserScroll.Y == 0) {
						webBrowserScroll.X = wb->Document->GetElementsByTagName("HTML")[0]->ScrollLeft;
						webBrowserScroll.Y = wb->Document->GetElementsByTagName("HTML")[0]->ScrollTop;
					}
				}

				// テキスト更新
				tb->Text = curHmHTMLTotalText;

				// 音の消去
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// テキスト更新
				wb->DocumentText = curHmHTMLTotalText;

				isDocumentChanged = true;
			}
		}
		catch (Exception^) {}
	}

	// テキスト更新が完了した時(最初の読み込み時もここに来る)
	void wb_DocumentCompleted(Object^ sender, WebBrowserDocumentCompletedEventArgs^ e)
	{
		try {
			if (isDocumentChanged) {
				isDocumentChanged = false;

				// 音を戻す
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

				// 保持していた座標へとスクロール
				wb->Document->Window->ScrollTo(webBrowserScroll.X, webBrowserScroll.Y);
			}
		}
		catch (Exception^) {}
	}
};


//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)


extern intHM_t Create();
MACRO_DLL intptr_t Show(HWND hWndHidemaru) {
	// 変な呼び覚まし方だが、hmJSのインターフェイスと合わせるため。
	Create();

	// インスタンスをフォーム自身に保持させて、フォームを表示。
	HmCustomLivePreviewForm::form = gcnew HmCustomLivePreviewForm(hWndHidemaru);
	HmCustomLivePreviewForm::form->Show();

	return TRUE;
}

MACRO_DLL intptr_t DestroyCustomLivePreview() {
	try {
		if (HmCustomLivePreviewForm::form != nullptr) {
			// 音を戻す
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			// 先にタイマーをストップ
			HmCustomLivePreviewForm::form->update->Stop();
			HmCustomLivePreviewForm::form->update->Enabled = false;

			// 閉じてNull
			HmCustomLivePreviewForm::form->Close();
			HmCustomLivePreviewForm::form = nullptr;

			// インタプリタの分の破棄
			(intHM_t)ICustomLivePreviewStaticLib::DestroyScope();

			// 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
			GC::Collect();
			GC::WaitForPendingFinalizers();
		}
	}
	catch (Exception^) {}

	return TRUE;
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	// インタプリタの分の破棄
	(intHM_t)ICustomLivePreviewStaticLib::DestroyScope();
	return DestroyCustomLivePreview();
}