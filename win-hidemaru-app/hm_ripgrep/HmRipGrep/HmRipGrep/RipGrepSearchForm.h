#pragma once
#include "convert_string.h"

using namespace std;
using namespace msclr::interop;
using namespace System::Collections::Generic;
using namespace System;
using namespace System::Text;
using namespace System::Text::RegularExpressions;
using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Drawing;


#include "RipGrepCommandLine.h"

namespace HmRipGrep {

	ref class RipGrepSearchForm : Form {
	public:
		static RipGrepSearchForm^ form;
	private:
		Button^ btnOK;
		Button^ btnCancel;
		TextBox^ tbSearchWord;
		TextBox^ tbTargetDir;
		HWND hWndOwner;
	public:
		RipGrepSearchForm(HWND hWndOwner, int OptionFlags) {
			this->hWndOwner = hWndOwner;
			this->OptionFlags = OptionFlags;
			SetFormAttribute();
			SetTextBoxSearchWordAttribute();
			SetTextBoxTargetDirAttribute();
			SetButtonAttribute();
			TaskList->Clear();
		}
	private:
		// フォームの属性
		void SetFormAttribute() {

			this->TopMost = true;
			this->Capture = true;
			this->StartPosition = FormStartPosition::Manual;
			this->MaximizeBox = false;
			this->Height =180;
			this->Width = 520;
			this->FormBorderStyle = ::FormBorderStyle::FixedSingle;
			this->Text = gcnew String("RipGrep検索");

			SetFormPositionFromOwnerHidemaruWindow();
		}
		// このフォーム自体を、秀丸の位置に応じて、表示位置を決めるため
		void SetFormPositionFromOwnerHidemaruWindow() {
			if ( IsWindow(this->hWndOwner) ) {
				// もし親がまだあるなら、そちらにすげ替え
				// ( hidemaruhandle(0) == Hidemaru32Class なのだが、秀丸が「タブモード」の時と「非タブモード」の時では
				// 実は実装が異なる。「タブモード」の時は、Hidemaru32Classは「秀丸の一番外側」と「エディタ部分付近の内側」の両方に「全く同じHidemaru32Classという名前」が付いていて
				// 親子のような関係になっている。
				// 一方「非タブモード」では、普通の作りで「一番外側のウィンドウ」が hidemaruhandle(0)である。
				HWND hWnd = GetParent(hWndOwner);
				if (hWnd) {
					wchar_t szBufClasName[MAX_CLASS_NAME];
					GetClassName(hWnd, szBufClasName, _countof(szBufClasName));
					if ( wcsstr(szBufClasName, L"Hidemaru32Class") ) {
						this->hWndOwner = hWnd;
					}
				}

				// 現在の秀丸の座標を元に、自分の表示位置を決める
				RECT rect;
				GetWindowRect(this->hWndOwner, &rect);
				this->Top = rect.top;
				this->Left = (rect.left + rect.right) / 2 - this->Width / 2;
			}
		}

		// テキストボックスの属性
		void SetTextBoxSearchWordAttribute() {
			// フォームとの隙間
			const int nPadding = 6;
			tbSearchWord = gcnew TextBox();
			tbSearchWord->Text = gcnew String("");
			tbSearchWord->Left = nPadding;
			tbSearchWord->Top = nPadding;
			tbSearchWord->Height = this->ClientSize.Height - nPadding * 2;
			tbSearchWord->Width = this->ClientSize.Width - nPadding * 2;
			tbSearchWord->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 20);

			// テキストボックスの中身が変わった時
			tbSearchWord->TextChanged += gcnew EventHandler(this, &RipGrepSearchForm::tbSearchWord_TextChanged);

			tbSearchWord->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &RipGrepSearchForm::tbSearchWord_KeyDown);

			this->Controls->Add(tbSearchWord);
		}

		// テキストボックスの属性
		void SetTextBoxTargetDirAttribute() {
			// フォームとの隙間
			const int nPadding = 6;
			tbTargetDir = gcnew TextBox();
			tbTargetDir->Text = gcnew String("");
			tbTargetDir->Left = nPadding;
			tbTargetDir->Top = tbSearchWord->Bottom + nPadding;
			tbTargetDir->Height = this->ClientSize.Height - nPadding * 2;
			tbTargetDir->Width = this->ClientSize.Width - nPadding * 2;
			tbTargetDir->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 20);

			// テキストボックスの中身が変わった時
			tbTargetDir->TextChanged += gcnew EventHandler(this, &RipGrepSearchForm::tbTargetDir_TextChanged);

			tbTargetDir->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &RipGrepSearchForm::tbTargetDir_KeyDown);

			this->Controls->Add(tbTargetDir);
		}


		void SetButtonAttribute() {
			btnOK = gcnew Button();
			btnOK->Text = L"OK";
			btnOK->Click += gcnew System::EventHandler(this, &RipGrepSearchForm::btnOK_Click);
			btnOK->Width = 150;
			btnOK->Height = 34;
			btnOK->Left = 50;
			btnOK->Top = 100;
			btnOK->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 16);
			btnCancel = gcnew Button();
			btnCancel->Text = L"キャンセル";
			btnCancel->Click += gcnew System::EventHandler(this, &RipGrepSearchForm::btnCancel_Click);
			btnCancel->Width = 150;
			btnCancel->Height = 34;
			btnCancel->Left = this->Width - btnCancel->Width - 50;
			btnCancel->Top = 100;
			btnCancel->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 16);

			this->Controls->Add(btnOK);
			this->Controls->Add(btnCancel);
		}

		List<Task^>^ TaskList = gcnew List<Task^>();
		void btnOK_Click(Object^ sender, EventArgs^ e) {

			if (tbSearchWord->Text->Length == 0) {
				return;
			}

			// 非同期で結果を秀丸へと反映するためのタスクを生成
			tbSearchWordText = tbSearchWord->Text;
			tbTargetDirText = tbTargetDir->Text;
			Task^ task = Task::Factory->StartNew(gcnew Action(this, &RipGrepSearchForm::TaskMethod));

			// リストの中で終わってるタスクはクリア
			for (int ix = TaskList->Count - 1; ix >= 0; ix--) {
				if (TaskList[ix]->IsCompleted) {
					TaskList->RemoveAt(ix);
				}
			}

			// 改めてタスクに加える
			TaskList->Add(task);

			btnOK->Enabled = false;
			tbSearchWord->Enabled = false;
		}

		void btnCancel_Click(Object^ sender, EventArgs^ e) {
			btnOK->Enabled = true;
			tbSearchWord->Enabled = true;
			Close();
		}


		// このウィンドウを表示した後、一度でもEverythingへとクエリーしたことがあるか
		// マウスを外でクリックとかした際に、閉じていいかどうかの基準に利用する
		bool isHaveQueriedFlag = false; 

		void tbSearchWord_KeyDown(Object^ sender, KeyEventArgs^ e) {
			if (e->KeyCode == System::Windows::Forms::Keys::Return) {
				if (tbSearchWord->Text->Length == 0) {
					btnCancel_Click(sender, e);
				}
				else {
					btnOK_Click(sender, e);
				}
			}
		}
		void tbTargetDir_KeyDown(Object^ sender, KeyEventArgs^ e) {
			if (e->KeyCode == System::Windows::Forms::Keys::Return) {
				if (tbTargetDir->Text->Length == 0) {
					btnCancel_Click(sender, e);
				}
				else {
					btnOK_Click(sender, e);
				}
			}
		}

		void tbSearchWord_TextChanged(Object^ sender, EventArgs^ e) {
			tbSearchWordText = tbSearchWord->Text;
		}
		void tbTargetDir_TextChanged(Object^ sender, EventArgs^ e) {
			tbTargetDirText = tbSearchWord->Text;
		}

	private:
		int OptionFlags = 0;
		BOOL inline IsOptionFlagsHasFile() {
			return OptionFlags & 0x01;
		}
		BOOL inline IsOptionFlagHasDir() {
			return OptionFlags & 0x02;
		}

	private:
		Mutex^ mut = gcnew Mutex();

		String^ tbSearchWordText = L"";
		String^ tbTargetDirText = L"";
		void TaskMethod() {

			wstring data = L"";

			int num = 100;

			if (data == L"") {
				data += L"対象母数" + to_wstring(num) + L"\n";
			}

			// ここでロック。最大でも0.5秒まち。
			if (mut->WaitOne(500)) {
				RipGrepCommanLine::Clear();
				RipGrepCommanLine::Grep(tbSearchWordText, tbTargetDirText, false);
				RipGrepCommanLine::Grep(tbSearchWordText, tbTargetDirText, true);
				// ロック解放
				mut->ReleaseMutex();

				Action^ delegateButtonEnable = gcnew Action(this, &RipGrepSearchForm::ButtonEnable);
				Action^ delegateTextBoxEnable = gcnew Action(this, &RipGrepSearchForm::TextBoxEnable);

				btnOK->Invoke(delegateButtonEnable);
				tbSearchWord->Invoke(delegateTextBoxEnable);
			}
		}

		void ButtonEnable() {
			btnOK->Enabled = true;

		}
		void TextBoxEnable() {
			tbSearchWord->Enabled = true;
		}

	public:
		void Cancel() {
			try {
				RipGrepCommanLine::Stop();
				Task::WaitAll(TaskList->ToArray());
				if (mut != nullptr) {
					delete mut;
				}
				
			}
			catch (Exception ^e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}
		void Stop() {
			Cancel();
			this->Close();
		}

	};
}

