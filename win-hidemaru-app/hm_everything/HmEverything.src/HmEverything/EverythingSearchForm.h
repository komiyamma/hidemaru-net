#pragma once

using namespace std;
using namespace msclr::interop;
using namespace System::Collections::Generic;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Drawing;


namespace HmEverything {

	wstring g_data = L"";

	ref class EverythingSearchForm : Form {
	public:
		static EverythingSearchForm^ form;
	private:
		TextBox^ tb;
		HWND hWndOwner;
	public:
		EverythingSearchForm(HWND hWndOwner, int OptionFlags) {
			this->hWndOwner = hWndOwner;
			this->OptionFlags = OptionFlags;
			SetFormAttribute();
			SetTextBoxAttribute();
		}
	public:
		void form_OnShown(Object^ o, EventArgs^ e) {
		}
	private:
		// フォームの属性
		void SetFormAttribute() {

			this->TopMost = true;
			this->Capture = true;
			this->StartPosition = FormStartPosition::Manual;
			this->MaximizeBox = false;
			this->Height = 80;
			this->Width = 240;
			this->FormBorderStyle = ::FormBorderStyle::FixedSingle;
			this->Text = gcnew String("Everything検索");
			this->Shown += gcnew System::EventHandler(this, &HmEverything::EverythingSearchForm::form_OnShown);

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
			const int nPadding = 2;
			tb = gcnew TextBox();
			tb->Text = gcnew String("");
			tb->Left = nPadding;
			tb->Height = this->ClientSize.Height - nPadding * 2;
			tb->Width = this->ClientSize.Width - nPadding * 2;
			tb->Font = gcnew System::Drawing::Font(gcnew String(L"ＭＳ ゴシック"), 20);

			// テキストボックス上で、「フォームに対して特殊なことをしてしまう」キー自体を、別に割り振るため
			tb->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &HmEverything::EverythingSearchForm::tb_PreviewKeyDown);

			// テキストボックスの中身が変わった時
			tb->TextChanged += gcnew EventHandler(this, &EverythingSearchForm::tb_TextChanged);

			this->Controls->Add(tb);
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

			isHaveQueriedFlag = true;
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

		void TaskMethod() {

			wstring data = L"";

			// 結果全てループしてリストボックスにそれぞれの結果を追加
			for (int i = 0; i < (int)Everything_GetNumResults(); i++)
			{
				// ファイルの時だけ
				// 各結果のフルパスとファイル名を
				wstring full = Everything_GetResultPath(i) + wstring(L"\\") + Everything_GetResultFileName(i);
				// 対象がファイルなら
				if (Everything_IsFileResult(i)) {
					// ファイルを表示するというオプションが有効なら
					if (IsOptionFlagsHasFile()) {
						// 表示
						full += L"\t\t(F)";
						data += full + wstring(L"\t\t") + Everything_GetResultFileName(i) + L"\n";
					}
				}
				// 対象がディレクトリなら
				else if (Everything_IsFolderResult(i)) {
					// 対象がディレクトリを表示するというオプションが有効なら
					if (IsOptionFlagHasDir()) {
						// 表示
						full += L"\t\t(D)";
						data += full + wstring(L"\t\t") + Everything_GetResultFileName(i) + L"\n";
					}
				}
				// それ以外なら
				else {
					// 表示
					full += L"\t\t(O)";
					data += full + wstring(L"\t\t") + Everything_GetResultFileName(i) + L"\n";
				}
			}

			// ここでロック。最大でも0.5秒まち。
			if (mut->WaitOne(500)) {
				// あほみたいだが秀丸が何かビジーになっていないかチェック。
				// あほみたいな処理だと目立たせるため、あえて、「激しくコピー感」を残しておく。
				// 最大0.3秒程度だが細かく
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

				// 大丈夫そうなら書き込み
				if ( ! CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					CHidemaruExeExport::SetTotalText(data);

					// TSVモードにする
					CHidemaruExeExport::EvalMacro(LR"(
						movetolineno 1, 0;
						config "xTabMode:0x0001";
					)");
				}
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

			// nIdパラメータ用にはデフォルトid (0)を使用
			// Everythingから結果が返ってきたなら
			if (Everything_IsQueryReply(m.Msg, wP, lP, MY_REPLY_ID))
			{
				// 非同期で結果を秀丸へと反映するためのタスクを生成
				Task^ task = Task::Factory->StartNew(gcnew Action(this, &EverythingSearchForm::TaskMethod));
			}

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
			Task::WaitAll();
			delete mut;
			this->Close();
		}

	};
}

