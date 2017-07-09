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
		TextBox^ tb;
		HWND hWndOwner;
	public:
		RipGrepSearchForm(HWND hWndOwner, int OptionFlags) {
			this->hWndOwner = hWndOwner;
			this->OptionFlags = OptionFlags;
			SetFormAttribute();
			SetTextBoxAttribute();
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
		void SetTextBoxAttribute() {
			// フォームとの隙間
			const int nPadding = 6;
			tb = gcnew TextBox();
			tb->Text = gcnew String("");
			tb->Left = nPadding;
			tb->Top = nPadding;
			tb->Height = this->ClientSize.Height - nPadding * 2;
			tb->Width = this->ClientSize.Width - nPadding * 2;
			tb->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 20);

			// テキストボックス上で、「フォームに対して特殊なことをしてしまう」キー自体を、別に割り振るため
			tb->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &RipGrepSearchForm::tb_PreviewKeyDown);

			// テキストボックスの中身が変わった時
			tb->TextChanged += gcnew EventHandler(this, &RipGrepSearchForm::tb_TextChanged);

			tb->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &RipGrepSearchForm::tb_KeyDown);

			this->Controls->Add(tb);
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

			if (tb->Text->Length == 0) {
				return;
			}

			// 非同期で結果を秀丸へと反映するためのタスクを生成
			tbText = tb->Text;
			Task^ task = Task::Factory->StartNew(gcnew Action(this, &RipGrepSearchForm::TaskMethod));

			// リストの中で終わってるタスクはクリア
			for (int ix = TaskList->Count - 1; ix >= 0; ix--) {
				if (TaskList[ix]->IsCompleted) {
					TaskList->RemoveAt(ix);
				}
			}

			// 改めてタスクに加える
			TaskList->Add(task);
		}

		void btnCancel_Click(Object^ sender, EventArgs^ e) {

			Stop();
		}

		void tb_PreviewKeyDown(Object^ sender, PreviewKeyDownEventArgs^ e) {

			// タブを押したらウィンドウを閉じる
			switch (e->KeyCode) {
				case Keys::Tab: {
					e->IsInputKey = true;
					Stop();
					break;
				}
			}
		}

		// このウィンドウを表示した後、一度でもEverythingへとクエリーしたことがあるか
		// マウスを外でクリックとかした際に、閉じていいかどうかの基準に利用する
		bool isHaveQueriedFlag = false; 

		void tb_KeyDown(Object^ sender, KeyEventArgs^ e) {
			if (e->KeyCode == System::Windows::Forms::Keys::Return) {
				if (tb->Text->Length == 0) {
					btnCancel_Click(sender, e);
				}
				else {
					btnOK_Click(sender, e);
				}
			}
		}

		void tb_TextChanged(Object^ sender, EventArgs^ e) {
			tbText = tb->Text;
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

		String^ tbText = L"";
		void TaskMethod() {

			wstring data = L"";

			int num = 100;

			if (data == L"") {
				data += L"対象母数" + to_wstring(num) + L"\n";
			}

			// ここでロック。最大でも0.5秒まち。
			if (mut->WaitOne(500)) {
				RipGrepCommanLine::Clear();
				RipGrepCommanLine::Grep(tbText, false);
				RipGrepCommanLine::Grep(tbText, true);
				// ロック解放
				mut->ReleaseMutex();
			}
		}

	protected:
		// Everythingの非同期の結果取得のために必要らしい
		const int MY_REPLY_ID = 0;

		virtual void WndProc(Message %m) override
		{
			WPARAM wP = (WPARAM)(m.WParam.ToPointer());
			LPARAM lP = (LPARAM)(m.LParam.ToPointer());


			// マウスをこのウィンドウの外でクリックしたりした場合は、処理を全部とめてウィンドウを閉じるための処理。
			if (m.Msg == WM_NCACTIVATE) {
				// 結果が出たことがある
				if (isHaveQueriedFlag) {
					// マウスがウィンドウの外である
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
			RipGrepCommanLine::Stop();
			Task::WaitAll(TaskList->ToArray());
			delete mut;
			this->Close();
		}

	};
}

