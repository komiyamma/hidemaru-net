/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
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

		wchar_t szCurrentFileFullPath[MAX_PATH * 2] = L""; // �G�ۂ�SetHidemaruHandle���Ă΂ꂽ���ɍŏ��ɃX���b�g�ɓ���t�@�C�����B�r���Ńt�@�C�������ς�邱�Ƃ�����̂ŁA�����܂ł��Ȃ������ꍇ�p�r

		if (!IsWindow(hWndHidemaru)) {
			hWndHidemaru = CHidemaruExeExport::GetCurWndHidemaru();   // ���݂̏G�ۃE�B���h�E�̃t�@�C�����𓾂�B
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

		this->Text = "�G�ۗp HmCustomLivePreview";

		// �A�Z���u���擾�`�A�C�R���ݒ�
		// System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		// System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.Resource", prj_assebmly->GetName()->Name), prj_assebmly);
		// System::Drawing::Icon^ icon = (System::Drawing::Icon^)(r->GetObject("md"));
		// this->Icon = icon;

		// ���̃t�H�[���T�C�Y
		this->Width = 600;
		this->Height = 600;
		sc->ClientSize = this->ClientSize;

		// ���ʂȂ����������A�ꉞWait�J�[�\���͎g��Ȃ�
		Application::UseWaitCursor = false;

		// ���钼�O
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
		// WebBrowser�I�u�W�F�N�g��z�u
		wb = gcnew WebBrowser();
		wb->Anchor = (AnchorStyles::Left | AnchorStyles::Top | AnchorStyles::Right | AnchorStyles::Bottom);
		wb->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler(this, &HmCustomLivePreviewForm::wb_DocumentCompleted);

		// �E���ɔz�u�B�ڂ����ς�
		wb->ClientSize = sc->Panel2->ClientSize;

		// �X�N���v�g�G���[�ł��_�C�A���O�Ƃ��o���Ȃ�
		wb->ScriptErrorsSuppressed = true;

		// �t�H�[���̃T�C�Y�ω��������Ǐ]
		this->SizeChanged += gcnew EventHandler(this, &HmCustomLivePreviewForm::form_SizeChanged);
		sc->Panel2->Controls->Add(wb);

	}

	void SetTimerAttribute() {
		// �P�b�ɂP��̍X�V�ŏ\�����낤�B
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmCustomLivePreviewForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	//=========================================================================================
	// �t�H�[�����[�h
	//=========================================================================================
private:

	void form_SizeChanged(Object^ o, EventArgs^ e) {
		try {
			sc->ClientSize = this->ClientSize;

			// �����̃T�C�Y����
			tb->ClientSize = sc->Panel1->ClientSize;

			// WebBrowser���E���S�̂ɁB
			wb->ClientSize = sc->Panel2->ClientSize;

		}
		catch (Exception^) {}
	}

	void sc_SplitterMoved(Object^ o, SplitterEventArgs^ e) {
		form_SizeChanged(nullptr, nullptr);
	}

	void form_FormClosing(Object^ o, FormClosingEventArgs^ e) {
		try {
			// �t�H�[���������������B
			update->Stop();
			update = nullptr;

			// ����߂�
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			// �C���^�v���^�̕��̔j��
			(intHM_t)ICustomLivePreviewStaticLib::DestroyScope();

			// �K�x�[�W�R���N�g�����Ă����B
			GC::Collect();
		}
		catch (Exception^) {}
	}

	Point webBrowserScroll = Point(0, 0);
	bool isDocumentChanged = false;
	//=========================================================================================
	// �^�C�}�[�C�x���g
	//=========================================================================================
	void update_Tick(Object ^sender, EventArgs ^e)
	{
		try {

			String^ curHmHTMLTotalText = ICustomLivePreviewStaticLib::OnCustomTranslateHTML(strCurFileFullPath);

			strCurFileFullPath = GetCurrentHidemaruFileName();

			// �e�L�X�g���ω��������A�������́A�t�@�C�������ω��������ɍX�V
			if (strPrvHmEditTotalText != curHmHTMLTotalText || strCurFileFullPath != strPrvFileFullPath) {

				strPrvHmEditTotalText = curHmHTMLTotalText;

				if (wb->Document != nullptr)
				{
					// ��i���̇@�D
					// Document->Body������p�^�[���B����ł͎��s����Ƃ�������B
					webBrowserScroll.X = wb->Document->Body->ScrollLeft;
					webBrowserScroll.Y = wb->Document->Body->ScrollTop;

					// ��i���̇A�D
					// HTML�G�������g��Scroll�ʒu�����ɍs���p�^�[���B����������s����Ƃ�������B
					if (webBrowserScroll.Y == 0) {
						webBrowserScroll.X = wb->Document->GetElementsByTagName("HTML")[0]->ScrollLeft;
						webBrowserScroll.Y = wb->Document->GetElementsByTagName("HTML")[0]->ScrollTop;
					}
				}

				// �e�L�X�g�X�V
				tb->Text = curHmHTMLTotalText;

				// ���̏���
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// �e�L�X�g�X�V
				wb->DocumentText = curHmHTMLTotalText;

				isDocumentChanged = true;
			}
		}
		catch (Exception^) {}
	}

	// �e�L�X�g�X�V������������(�ŏ��̓ǂݍ��ݎ��������ɗ���)
	void wb_DocumentCompleted(Object^ sender, WebBrowserDocumentCompletedEventArgs^ e)
	{
		try {
			if (isDocumentChanged) {
				isDocumentChanged = false;

				// ����߂�
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

				// �ێ����Ă������W�ւƃX�N���[��
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
	// �ςȌĂъo�܂��������AhmJS�̃C���^�[�t�F�C�X�ƍ��킹�邽�߁B
	Create();

	// �C���X�^���X���t�H�[�����g�ɕێ������āA�t�H�[����\���B
	HmCustomLivePreviewForm::form = gcnew HmCustomLivePreviewForm(hWndHidemaru);
	HmCustomLivePreviewForm::form->Show();

	return TRUE;
}

MACRO_DLL intptr_t DestroyCustomLivePreview() {
	try {
		if (HmCustomLivePreviewForm::form != nullptr) {
			// ����߂�
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			// ��Ƀ^�C�}�[���X�g�b�v
			HmCustomLivePreviewForm::form->update->Stop();
			HmCustomLivePreviewForm::form->update->Enabled = false;

			// ����Null
			HmCustomLivePreviewForm::form->Close();
			HmCustomLivePreviewForm::form = nullptr;

			// �C���^�v���^�̕��̔j��
			(intHM_t)ICustomLivePreviewStaticLib::DestroyScope();

			// �G�ۂ�����O�܂łɉ\�Ȍ���}�l�[�W�h�̃C���X�^���X��������ĕs���G���[���o�ɂ�����ԂւƎ����Ă����B
			GC::Collect();
			GC::WaitForPendingFinalizers();
		}
	}
	catch (Exception^) {}

	return TRUE;
}

MACRO_DLL intHM_t DllDetachFunc_After_Hm866() {
	// �C���^�v���^�̕��̔j��
	(intHM_t)ICustomLivePreviewStaticLib::DestroyScope();
	return DestroyCustomLivePreview();
}