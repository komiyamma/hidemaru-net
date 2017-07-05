#pragma once

#include "CaptureReagion.h"

using namespace std;
using namespace msclr::interop;
using namespace System::Collections::Generic;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Drawing;

wstring g_data = L"";

namespace HmEverything {
	ref class EverythingSearchForm : Form {
		int FilterMode = 0;
	public:
		static EverythingSearchForm^ form;
	private:
		TextBox^ tb;
		HWND hWndOwner;
	public:
		EverythingSearchForm(HWND hWndOwner, int FilterMode) {
			this->Capture = true;
			this->hWndOwner = hWndOwner;
			this->FilterMode = FilterMode;
			this->MouseDown += gcnew MouseEventHandler(this, &EverythingSearchForm::form_MouseDown);
			this->TopMost = true;
			SetFormAttribute();
			SetTextBoxAttribute();
		}
	private:
		void form_MouseDown(Object^ o, MouseEventArgs^ e) {
		}

	public:
		void form_OnShown(Object^ o, EventArgs^ e) {
		}
	private:
		void SetFormAttribute() {
			this->StartPosition = FormStartPosition::Manual;
			this->Height = 80;
			this->Width = 240;
			this->Text = gcnew String("Everything検索");
			this->Shown += gcnew System::EventHandler(this, &HmEverything::EverythingSearchForm::form_OnShown);

			SetFormPositionFromOwnerHidemaruWindow();
		}
		void SetFormPositionFromOwnerHidemaruWindow() {
			if ( IsWindow(this->hWndOwner) ) {
				// もし親がまだあるなら、そちらにすげ替え
				HWND hWnd = GetParent(hWndOwner);
				if (hWnd) {
					wchar_t szBufClasName[MAX_CLASS_NAME];
					GetClassName(hWnd, szBufClasName, _countof(szBufClasName));
					if ( wcsstr(szBufClasName, L"Hidemaru32Class") ) {
						this->hWndOwner = hWnd;
					}
				}
				RECT rect;
				GetWindowRect(this->hWndOwner, &rect);
				this->Top = rect.top;
				this->Left = (rect.left + rect.right) / 2 - this->Width / 2;
			}
		}

		void SetTextBoxAttribute() {
			const int nPadding = 2;
			tb = gcnew TextBox();
			tb->Text = gcnew String("");
			tb->Left = nPadding;
			tb->Height = this->ClientSize.Height - nPadding * 2;
			tb->Width = this->ClientSize.Width - nPadding * 2;
			tb->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 20);
			tb->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &HmEverything::EverythingSearchForm::tb_PreviewKeyDown);
			tb->TextChanged += gcnew EventHandler(this, &EverythingSearchForm::tb_TextChanged);
			this->Controls->Add(tb);
		}

		void tb_PreviewKeyDown(Object^ sender, PreviewKeyDownEventArgs^ e) {
			switch (e->KeyCode) {
			case Keys::Tab: {
				e->IsInputKey = true;
				Stop();
				}
			}
		}

		bool isQueryFlag = false;
		void tb_TextChanged(Object^ o, EventArgs^ e) {
			wstring wstr = marshal_as<wstring>(tb->Text);

			// 検索文字列として abc を指定
			Everything_SetSearch(wstr.c_str());

			// 応答ウィンドウ(このウィンドウ)を設定 [Everything_Queryで結果を待機中の場合は必須]
			Everything_SetReplyWindow((HWND)this->Handle.ToPointer());

			// 応答IDを設定
			Everything_SetReplyID(MY_REPLY_ID);

			// 可視結果ウィンドウをセットアップ
			Everything_SetMax(200);

			// 可視結果ウィンドウをセットアップ(何番めから表示するのか)
			// Everything_SetOffset(0);

			// Everythingへ問合せを実行
			Everything_Query(false);

			isQueryFlag = true;
		}

	private:
		Mutex^ mut = gcnew Mutex();

		void TaskMethod() {
			int i;

			wstring data = L"";

			// 結果全てループしてリストボックスにそれぞれの結果を追加
			for (i = 0; i < Everything_GetNumResults(); i++)
			{
				// ファイルの時だけ
				// 各結果のフルパスとファイル名を
				wstring full = Everything_GetResultPath(i) + wstring(L"\\") + Everything_GetResultFileName(i);
				if (Everything_IsFileResult(i)) {
					if (FilterMode & 0x1) {
						full += L"\t\t(F)";
						data += full + wstring(L"\t\t") + Everything_GetResultFileName(i) + L"\n";
					}
				}
				else {
					if (FilterMode & 0x2) {
						full += L"\t\t(D)";
						data += full + wstring(L"\t\t") + Everything_GetResultFileName(i) + L"\n";
					}
				}
			}

			if (mut->WaitOne(500)) {

				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
				}
				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
				}
				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
				}
				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
				}
				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
				}
				if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					Threading::Thread::Sleep(50);
				}
				if ( ! CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					CHidemaruExeExport::SetTotalText(data);

					// TSVモードにする
					CHidemaruExeExport::EvalMacro(LR"(
						movetolineno 1, 0;
						config "xTabMode:0x0001";
					)");
				}
				mut->ReleaseMutex();
			}
		}

	protected:
		const int MY_REPLY_ID = 0;

		virtual void WndProc(Message %m) override
		{
			WPARAM wP = (WPARAM)(m.WParam.ToPointer());
			LPARAM lP = (LPARAM)(m.LParam.ToPointer());

			System::Diagnostics::Trace::WriteLine(m.Msg.ToString());

			// nIdパラメータ用にはデフォルトid (0)を使用
			if (Everything_IsQueryReply(m.Msg, wP, lP, MY_REPLY_ID))
			{
				Task^ task = Task::Factory->StartNew(gcnew Action(this, &EverythingSearchForm::TaskMethod));
			}

			if (m.Msg == WM_NCACTIVATE) {
				if (isQueryFlag) {

					System::Diagnostics::Trace::WriteLine("マウスのところ");
					int x = Cursor->Position.X;
					int y = Cursor->Position.Y;

					if (x < this->Location.X || this->Location.X + this->Width < x) {
						Stop();
					}

					if (y < this->Location.Y || this->Location.Y + this->Height < y) {
						Stop();
					}
				}
			}

			System::Windows::Forms::Form::WndProc(m);

		}
	public:
		void Stop() {
			Task::WaitAll();
			delete mut;
			this->Close();
		}

	};
}

