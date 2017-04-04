#include "HmExeExport.h"

using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Net;

CHidemaruExeExport HMEXE;

#pragma comment(lib, "urlmon")


public ref class HmMarkDownPreviewForm : System::Windows::Forms::Form
{
public:
	static HmMarkDownPreviewForm^ form;
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

	HmMarkDownPreviewForm(HWND hWndHidemaru)
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

		sc->SplitterMoved += gcnew SplitterEventHandler(this, &HmMarkDownPreviewForm::sc_SplitterMoved);
	}

	String^ GetCurrentHidemaruFileName() {
		strPrvFileFullPath = strCurFileFullPath;
		const int WM_HIDEMARUINFO = WM_USER + 181;
		const int HIDEMARUINFO_GETFILEFULLPATH = 4;

		wchar_t szCurrentFileFullPath[MAX_PATH * 2] = L""; // 秀丸のSetHidemaruHandleが呼ばれた時に最初にスロットに入るファイル名。途中でファイル名が変わることがあるので、あくまでもなかった場合用途

														   // 現在の秀丸ウィンドウのファイル名を得る。
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

		this->Text = "秀丸用 HmMarkDownPreview";

		// アセンブリ取得～アイコン設定
		System::Reflection::Assembly^ prj_assebmly = GetType()->Assembly;
		System::Resources::ResourceManager^ r = gcnew System::Resources::ResourceManager(String::Format("{0}.Resource", prj_assebmly->GetName()->Name), prj_assebmly);
		System::Drawing::Icon^ icon = (System::Drawing::Icon^)(r->GetObject("md"));
		this->Icon = icon;

		// このフォームサイズ
		this->Width = 600;
		this->Height = 600;
		sc->ClientSize = this->ClientSize;

		// 効果なさそうだが、一応Waitカーソルは使わない
		Application::UseWaitCursor = false;

		// 閉じる直前
		this->FormClosing += gcnew FormClosingEventHandler(this, &HmMarkDownPreviewForm::form_FormClosing);
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
		wb->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler(this, &HmMarkDownPreviewForm::wb_DocumentCompleted);

		// 右側に配置。目いっぱい
		wb->ClientSize = sc->Panel2->ClientSize;

		// スクリプトエラーでもダイアログとか出さない
		wb->ScriptErrorsSuppressed = true;

		// フォームのサイズ変化した時追従
		this->SizeChanged += gcnew EventHandler(this, &HmMarkDownPreviewForm::form_SizeChanged);
		sc->Panel2->Controls->Add(wb);
		

	}

	void SetTimerAttribute() {
		// １秒に１回の更新で十分だろう。
		update = gcnew Timer();
		update->Interval = 1000 * 1;
		update->Tick += gcnew EventHandler(this, &HmMarkDownPreviewForm::update_Tick);
		update->Start();

		update_Tick(nullptr, nullptr);
	}

	//=========================================================================================
	// フォーム
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
			String^ curHmEditTotalText = gcnew String(HMEXE.GetTotalText().c_str());

			strCurFileFullPath = GetCurrentHidemaruFileName();

			// テキストが変化した時、もしくは、ファイル名が変化した時に更新
			if (strPrvHmEditTotalText != curHmEditTotalText || strCurFileFullPath != strPrvFileFullPath) {

				strPrvHmEditTotalText = curHmEditTotalText;

				Markdown^ md = gcnew Markdown();

				// 現在の編集の内容を取得
				String^ curHmHTMLTotalText = md->Transform(curHmEditTotalText);

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

				// ファイル名が有効であるならば、相対を変換する
				if (strCurFileFullPath->Length > 0) {
					// 相対→絶対
					curHmHTMLTotalText = RelativeToAbsolute(curHmHTMLTotalText);
				}

				// 音の消去
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// テキスト更新
				wb->DocumentText = curHmHTMLTotalText;
				isDocumentChanged = true;
			}
		}
		catch (Exception^ err) {
			System::Diagnostics::Debug::WriteLine(err->Message + "\n");
		}
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

	//=========================================================================================
	// 実体のないファイルの場合、画像やリンクは相対アドレスは不可能なので、絶対アドレスへと置き換える。
	//=========================================================================================
	String^ RelativeToAbsolute(String^ html) {

		try {
			String^ fn = strCurFileFullPath;
			fn = fn->Replace("\\", "/");
			String ^strBaseUrl = fn;
			HtmlAgilityPack::HtmlDocument ^doc = gcnew HtmlAgilityPack::HtmlDocument();
			doc->LoadHtml(html);

			bool isExistBaseHref = false;
			for each (auto base in doc->DocumentNode->Descendants("base"))
			{
				if (base->Attributes["href"]->Value->Length > 0) {
					isExistBaseHref = true;
				}
				else {

				}
			}

			// base hrefの指定が無いのであれば、現在の開いているファイルのディレクトリをそこにしておくことで、相対ディレクトリをフォローする。
			if (!isExistBaseHref) {
				String^ basedir = System::IO::Path::GetDirectoryName(strBaseUrl);
				HtmlAgilityPack::HtmlNode^ base = HtmlAgilityPack::HtmlNode::CreateNode("<base href=''>");
				base->Attributes["href"]->Value = basedir + "\\";

				// Headタグがあればそこにたす
				HtmlAgilityPack::HtmlNode^ head = doc->DocumentNode->SelectSingleNode("/html/head");
				HtmlAgilityPack::HtmlNode^ html = doc->DocumentNode->SelectSingleNode("/html");
				if (head) {
					head->PrependChild(base);
				}
				else if (html) {
					html->PrependChild(base);
				}
				// Headタグがないなら、トップにたさざるをえないだろう
				else {
					doc->DocumentNode->PrependChild(base);
				}
			}

			/*
			if (!isExistBaseHref) {
				auto objBaseURI = gcnew Uri(strBaseUrl);
				for each (auto img in doc->DocumentNode->Descendants("img"))
				{
				img->Attributes["src"]->Value = (gcnew Uri(objBaseURI, img->Attributes["src"]->Value))->AbsoluteUri;
				}

				for each (auto a in doc->DocumentNode->Descendants("a"))
				{
				a->Attributes["href"]->Value = (gcnew Uri(objBaseURI, a->Attributes["href"]->Value))->AbsoluteUri;
				}

				for each (auto script in doc->DocumentNode->Descendants("script"))
				{
				script->Attributes["src"]->Value = (gcnew Uri(objBaseURI, script->Attributes["src"]->Value))->AbsoluteUri;
				}

				for each (auto link in doc->DocumentNode->Descendants("link"))
				{
				link->Attributes["href"]->Value = (gcnew Uri(objBaseURI, link->Attributes["href"]->Value))->AbsoluteUri;
				}
			}
			*/

			StringWriter ^writer = gcnew StringWriter();
			doc->Save(writer);

			String ^newHtml = writer->ToString();
			return newHtml;
		}
		catch (Exception^ err) {
		}

		return html;
	}
};


//------------------------------------------------------------------------------------
#define MACRO_DLL extern "C" __declspec(dllexport)


MACRO_DLL intptr_t Show(HWND hWndHidemaru) {
	// インスタンスをフォーム自身に保持させて、フォームを表示。
	HmMarkDownPreviewForm::form = gcnew HmMarkDownPreviewForm(hWndHidemaru);
	HmMarkDownPreviewForm::form->Show();

	return TRUE;
}

MACRO_DLL intptr_t Dispose() {

	try {
		if (HmMarkDownPreviewForm::form != nullptr) {
			// 音を戻す
			CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, false);

			// 先にタイマーをストップ
			HmMarkDownPreviewForm::form->update->Stop();
			HmMarkDownPreviewForm::form->update->Enabled = false;

			// 閉じてNull
			HmMarkDownPreviewForm::form->Close();
			HmMarkDownPreviewForm::form = nullptr;

			// 秀丸が閉じる前までに可能な限りマネージドのインスタンスを解放して不正エラーが出にくい状態へと持っていく。
			GC::Collect();
			GC::WaitForPendingFinalizers();
		}
	}
	catch (Exception^) {}

	return TRUE;
}


MACRO_DLL intptr_t DllDetachFunc_After_Hm866() {
	return Dispose();
}