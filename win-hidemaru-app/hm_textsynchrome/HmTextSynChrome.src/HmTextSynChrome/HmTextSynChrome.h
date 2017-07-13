#pragma once

#include "hidemaruexe_export.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;
using namespace System::Web;
using namespace System::Collections::Generic;
using namespace System::Threading::Tasks;

using namespace OpenQA::Selenium;
using namespace OpenQA::Selenium::Chrome;

CHidemaruExeExport HMEXE;


/// やたらめったらtry_catchなのは、「いつ不意に秀丸本体が閉じても可能な限り黙って終了できるようにするため」
enum class hmHtmlPreviewMode { Live = 0, File = 1 };


/// <summary> モードに関わらず共通 </summary>
public ref class HmTextSynChromeForm : System::Windows::Forms::Form
{
public:
	static HmTextSynChromeForm^ form;
protected:
	HWND hWndHidemaru = NULL;
	String^ strPrvTotalText = "";
	String^ strOption1 = "--lang=jp";
	hmHtmlPreviewMode mode;
	Timer^ update;

protected:
	IWebDriver^ driver;
	ChromeOptions^ chromeOptions;
	ChromeDriverService^ chromeService;
	bool is_driver_ready = false;

public:

	HmTextSynChromeForm(HWND hWndHidemaru, String^ strOption1)
	{
		this->hWndHidemaru = hWndHidemaru;
		this->strOption1 = strOption1;

		SetFormAttribute();
		SetWebBrowserAttribulte();
		SetTimerAttribute();
	}

protected:

	/// <summary>フォームの属性設定</summary>
	void SetFormAttribute() {

		// 効果なさそうだが、一応Waitカーソルは使わない
		Application::UseWaitCursor = false;
	}

	/// <summary>Webブラウザ属性設定</summary>
	void SetWebBrowserAttribulte() {

		try {
			chromeOptions = gcnew ChromeOptions();
			System::Diagnostics::Trace::WriteLine("\n■" + strOption1);
			chromeOptions->AddArgument("--lang=" + strOption1);
			chromeService = ChromeDriverService::CreateDefaultService();
			chromeService->HideCommandPromptWindow = true;

			driver = gcnew ChromeDriver(chromeService, chromeOptions);
			driver->Url = L"about:blank";
			is_driver_ready = true;
		}
		catch (Exception^ e) {
			System::Diagnostics::Debug::WriteLine(e->Message);
		}
	}

	void SetTimerAttribute() {
		// １秒に１回の更新で十分だろう。
		update = gcnew System::Windows::Forms::Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmTextSynChromeForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	void TaskMethod() {
		String^ curTotalText = gcnew String(CHidemaruExeExport::GetTotalText().c_str());
		if (curTotalText != strPrvTotalText) {
			strPrvTotalText = curTotalText;

			// var body = driver.FindElement(By.TagName("body")); 
			auto body = driver->FindElement(By::TagName(L"body"));

			// var executor = driver as IJavaScriptExecutor;
			IJavaScriptExecutor^ executor = dynamic_cast<IJavaScriptExecutor^>(driver);


			// まず、JavaScriptによるEvaluateの予定なので、それを邪魔する要素は置き換え
			String^ encodeText = curTotalText->Replace(R"(\)", R"(\\)");
			encodeText = encodeText->Replace("'", R"(\')")->Replace(R"(")", R"(\")");

			// 改行を糾合
			encodeText = encodeText->Replace("\r\n", "\r");
			encodeText = encodeText->Replace("\n", "\r");
			encodeText = encodeText->Replace("\r", R"(\n)");

			System::Diagnostics::Trace::WriteLine(encodeText);
			// JavaScriptとして評価
			executor->ExecuteScript("arguments[0].innerText = '" + encodeText + "'", body);
		}
	}

	List<Task^>^ TaskList = gcnew List<Task^>();
	void update_Tick(Object ^sender, EventArgs ^e) {
		if (!is_driver_ready) {
			return;
		}
		try  {
			// 非同期で結果を秀丸へと反映するためのタスクを生成
			Task^ task = Task::Factory->StartNew(gcnew Action(this, &HmTextSynChromeForm::TaskMethod));

			// リストの中で終わってるタスクはクリア
			for (int ix = TaskList->Count - 1; ix >= 0; ix--) {
				if (TaskList[ix]->IsCompleted) {
					TaskList->RemoveAt(ix);
				}
			}

			// 改めてタスクに加える
			TaskList->Add(task);
		}
		catch (Exception^) {}
	}

public:
	virtual void Stop() {

		Task::WaitAll(TaskList->ToArray());
		try {
			if (update != nullptr) {
				// フォームを閉じた時も解放。
				update->Stop();
				update = nullptr;
			}
			if (driver != nullptr) {
				driver->Close();
				driver->Quit();
			}
		}
		catch (Exception^) {}
	}
};

