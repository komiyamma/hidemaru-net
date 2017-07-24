#pragma once

#include "HmHtmlBase.h"

public ref class HmHtmlLiveForm : public HmHtmlBaseForm {
public:
	HmHtmlLiveForm(HWND hWndHidemaru) : HmHtmlBaseForm(hWndHidemaru) {	}

protected:
	/// <summary>フォーム表示時にWebBrowserに流す情報。
	/// Liveモードの時は、Tickでやってることと同じことをする。
	///</summary>
	virtual  void wb_DocumentInit() override {
		try {
			update_Tick(nullptr, nullptr);
		}
		catch (Exception^) {}
	}

	/// <summary>Liveモード用のタイマーイベント
	/// 座標を保存、相対ディレクトリの内容を絶対ディレクトリへと修正、音を消して、テキストを更新する。
	/// テキストが更新された後は、webBrowser_Compo
	///</summary>
	virtual void update_Tick_Implements(Object ^sender, EventArgs ^e) override {
		try {
			strCurFileFullPath = GetCurrentHidemaruFileName();

			// 現在の編集の内容を取得
			String^ curHmEditTotalText = gcnew String(HMEXE.GetTotalText().c_str());

			// テキストが変化した時、もしくは、ファイル名が変化した時に更新
			if (strPrvHmEditTotalText != curHmEditTotalText || strCurFileFullPath != strPrvFileFullPath) {
				strPrvHmEditTotalText = curHmEditTotalText;

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

				// ファイル名が有効であるならば、相対を変換する
				if (strCurFileFullPath->Length > 0) {
					// 相対→絶対
					curHmEditTotalText = RelativeToAbsolute(curHmEditTotalText);
				}

				// 音の消去
				CoInternetSetFeatureEnabled(FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_PROCESS, true);

				// テキスト更新
				wb->DocumentText = curHmEditTotalText;
				isDocumentChanged = true;

			}
		}
		catch (Exception^ err) {
		}
	}


private:
	/// <summary>
	/// 事実上Liveモードのみ通過する。
	/// 実体のないファイルの場合、画像やリンクは相対アドレスは不可能なので、絶対アドレスへと置き換える。
	/// </summary>
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
				if (head) {
					head->PrependChild(base);
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
		catch (Exception^) {}

		return html;
	}

};
