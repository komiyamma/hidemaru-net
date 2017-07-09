#pragma once

#pragma comment(lib, "winmm")

namespace HmRipGrep {

	wstring g_data = L"";

	ref class RipGrepCommanLine {
		static System::Diagnostics::Process^ p;
	public:
		static void Clear() {
			if (hs != nullptr) {
				hs->Clear();
				// 大丈夫そうなら書き込み
				if (!CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
					CHidemaruExeExport::SetTotalText(L"");
				}
			}
		}
		static bool isStop = false;
		static void Stop() {
			if (p) {
				try {
					isStop = true;
					p->Close();
					p->Kill();
					delete p;
				}
				catch (Exception ^e) {

				}
			}
		}

		static bool m_is_add = false;
		static Mutex^ mut = gcnew Mutex();

		static void Grep(String^ searcText, String^ dirText, bool is_add)
		{
			try {
				m_is_add = is_add;

				// 追加モードなのにStopかかっていたら、何もしない
				if (m_is_add && isStop) {
					return;
				}

				isStop = false;

				//Processオブジェクトを作成する
				p = gcnew System::Diagnostics::Process();
				//起動するファイルを指定する
				p->StartInfo->FileName = gcnew String(CSelfDllInfo::GetSelfModuleDir().c_str()) + L"\\rg.exe";

				List<String^>^ list = gcnew List<String^>();
				if (m_is_add) {
					list->Add("-E");
					list->Add("utf8");
				}
				list->Add("--no-ignore");
				list->Add("-n");
				list->Add("-e");
				list->Add(Regex::Escape(searcText));
				list->Add("-S");
				list->Add(dirText);

				String^ arg_line = EncodeCommandLineValues(list);
				p->StartInfo->Arguments = arg_line;

				p->StartInfo->CreateNoWindow = true;
				p->StartInfo->UseShellExecute = false;

				//イベントハンドラの追加
				p->StartInfo->RedirectStandardError = true;
				p->StartInfo->RedirectStandardOutput = true;

				p->ErrorDataReceived += gcnew System::Diagnostics::DataReceivedEventHandler(&P_ErrorDataReceived);
				p->OutputDataReceived += gcnew System::Diagnostics::DataReceivedEventHandler(&P_OutputDataReceived);
				p->StartInfo->StandardOutputEncoding = Encoding::UTF8;
				p->StartInfo->StandardErrorEncoding = Encoding::UTF8;

				p->Exited += gcnew System::EventHandler(&p_Exited);
				//プロセスが終了したときに Exited イベントを発生させる
				p->EnableRaisingEvents = true;

				//起動する
				p->Start();
				p->BeginOutputReadLine();

				startTime = timeGetTime();

				p->WaitForExit();

				try {
					if (p) {
						p->Close();
						p->Kill();
					}
				}
				catch (Exception^ e) {

				}
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}

	private:
		static String^ alldata = "";
		static int cnt = 0;
		static DWORD startTime = 0;
		static Dictionary<String^, Boolean>^ hs = gcnew Dictionary<String^, Boolean>();
		static Regex^ r = gcnew Regex(R"MATCH(^[\s\S]+?:\d+:)MATCH");
		static const int nInterValMilliSecond = 1000;

		static void AddTotalText(wstring data) {
			if (isStop) {
				return;
			}

			try {
				if (mut->WaitOne(500)) {

					if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
						Threading::Thread::Sleep(50);
						if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
							Threading::Thread::Sleep(50);
							if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
								Threading::Thread::Sleep(50);
								if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
									Threading::Thread::Sleep(50);
									if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
										Threading::Thread::Sleep(50);
										if (CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
											Threading::Thread::Sleep(50);
										}
									}
								}
							}
						}
					}

					// 大丈夫そうなら書き込み
					if (!CHidemaruExeExport::Hidemaru_CheckQueueStatus()) {
						CHidemaruExeExport::AddTotalText(data);
					}

					// ロック解放
					mut->ReleaseMutex();
				}

			}
			catch (Exception ^e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}

		}

		static void P_OutputDataReceived(Object^ sender, System::Diagnostics::DataReceivedEventArgs^ e)
		{
			if (isStop) {
				return;
			}
			String^ data = e->Data;
			if (data == nullptr) {
				return;
			}

			try {

				String^ s = r->Match(data)->Value;
				if (s != nullptr) {
					bool is_must_add = false;
					if (m_is_add) {
						// 今度は、まだ登録されていない時だけ、SJIS版を吐き出す
						if (!hs->ContainsKey(s)) {
							is_must_add = true;
						}
					}
					else {
						hs[s] = true;
						is_must_add = true;
					}

					if (is_must_add) {
						alldata += data + L"\n";
					}

				}

				DWORD currentTime = timeGetTime();

				if (currentTime - startTime > nInterValMilliSecond) {
					startTime = currentTime;
					wstring data = String_to_wstring(alldata);
					alldata = "";

					// 大丈夫そうなら書き込み
					AddTotalText(data);
				}
			}
			catch (Exception ^e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}
		}

		static void P_ErrorDataReceived(Object^ sender, System::Diagnostics::DataReceivedEventArgs^ e)
		{
			// P_OutputDataReceived(sender, e);
		}

		static void p_Exited(Object^ sender, EventArgs^ e)
		{
			try {
				wstring data = String_to_wstring(alldata);
				alldata = "";

				AddTotalText(data);
				if (m_is_add) {
					AddTotalText(L"検索終了");
				}
			}
			catch (Exception^ e) {
				System::Diagnostics::Trace::WriteLine(e->Message);
			}

			// まぁ大きいだろうから早めに
			if (m_is_add) {
				hs->Clear();
			}
		}
		/// 
		/// コマンドライン引数 1 個をエンコード
		/// 
	public:
		static String^ EncodeCommandLineValue(String^ value)
		{
			if (String::IsNullOrEmpty(value))
			{
				return "";
			}
			auto containsSpace = value->IndexOfAny(gcnew cli::array<Char> {' ', '\t'}) != -1;

			// 「\…\"」をエスケープ
			// やってることは、「"」直前の「\」の数を 2倍+1
			value = _commandLineEscapePattern->Replace(value, R"($1\$&)");

			// スペース／タブが含まれる場合はデリミタで囲み、末尾が「\」だった場合、エスケープ
			if (containsSpace)
			{
				value = "\"" + _lastBackSlashPattern->Replace(value, "$1$1") + "\"";
			}
			return value;
		}
	private:
		static initonly Regex^ _commandLineEscapePattern = gcnew Regex("(\\\\*)\"");
		static initonly Regex^ _lastBackSlashPattern = gcnew Regex(R"((\\ + )$)");

		/// 
		/// コマンドライン引数複数個をエンコードして、スペースで結合
		/// 
	public:
		static String^ EncodeCommandLineValues(IEnumerable<String^>^ values)
		{
			if (values == nullptr)
			{
				throw gcnew ArgumentNullException("values");
			}
			String^ ret;
			for each (String^ str in values) {
				str = EncodeCommandLineValue(str);
				ret = ret + str + " ";
			}

			return ret;
		}
	};

}