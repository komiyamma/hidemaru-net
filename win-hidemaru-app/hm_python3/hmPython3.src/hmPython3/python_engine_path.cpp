#include "self_dll_info.h"
#include "convert_string.h"

#include <shlwapi.h>
#include "tinyxml.h"

#include "patchlevel.h"

/*
TiXmlDocument::LoadFile…ファイルを読み込む
・TiXmlDocument::RootElement…XML木構造のルートの要素を取得する
・TiXmlElement::FirstChildElement…指定した要素が持つ子の要素の1つ目を取得する
・TiXmlElement::NextSiblingElement…FirstChildElementの後に使うことで，次の要素を取得する
・TiXmlElement::Value…指定した要素から<tag a="str">777</tag>のtagを取得する
・TiXmlElement::GetText…指定した要素から<tag a="str">777</tag>の777を取得する
・TiXmlElement::Attribute…指定した要素から<tag a="str">777</tag>のaを指定した場合，strを取得する
*/
const wstring xmlFileName = L"hmPython3.xml";
const wstring wstrTargetKeyName = L"PYTHONPATH";
const wstring defaultRelativePath = L"\\hmPython3";

static wstring LoadPythonSettingsPathFromXml() {
	wstring xmlpath = CSelfDllInfo::GetSelfModuleDir() + L"\\" + xmlFileName;

	if (PathFileExists(xmlpath.c_str())) {
		string cp932_xmlpath = utf16_to_cp932(xmlpath);
		try {
			TiXmlDocument doc(cp932_xmlpath.c_str());
			doc.LoadFile();

			auto nodeConfiguration = doc.FirstChildElement("configuration");
			if (!nodeConfiguration) {
				MessageBox(NULL, L"XMLのconfiguration構成に不具合があります。", L"XMLのconfiguration構成に不具合があります。", NULL);
				return L"";
			}

			auto nodeAppSettings = nodeConfiguration->FirstChildElement("appSettings");
			if (!nodeAppSettings) {
				MessageBox(NULL, L"XMLのappSettings構成に不具合があります。", L"XMLのappSettings構成に不具合があります。", NULL);
				return L"";
			}

			TiXmlElement *elementAdd = nodeAppSettings->FirstChildElement("add");
			if (!elementAdd) {
				MessageBox(NULL, L"XMLのadd構成に不具合があります。", L"XMLのadd構成に不具合があります。", NULL);
				return L"";
			}
			while (true) {
				if (!elementAdd) {
					break;
				}
				auto key = elementAdd->Attribute("key");
				auto value = elementAdd->Attribute("value");
				if (key && value) {
					wstring wkey = cp932_to_utf16(key);
					wstring wvalue = cp932_to_utf16(value);
					if (wkey == wstrTargetKeyName) {
						return wvalue;
					}
				}

				elementAdd = elementAdd->NextSiblingElement();
			}
		}
		catch (exception e) {
			MessageBoxA(NULL, e.what(), e.what(), NULL);
		}
		return L"";

	}

	// XMLファイルは存在しない
	else {
		wstring defaultAbsolutePath = CSelfDllInfo::GetSelfModuleDir() + defaultRelativePath;
		// デフォルトでインストールしているHmPythonフォルダがあり、その中に、
		if ( PathFileExists(defaultAbsolutePath.c_str()) ) {
			return defaultAbsolutePath;
		}

		return L"";
	}

}

namespace PythonEngine {

	wstring GetPythonPath() {

		// XMLに設定されているPYTHONPATHの値
		wstring path_from_settings = L"";
		
		try {
			path_from_settings = LoadPythonSettingsPathFromXml();
		}
		catch (exception e) {

		}

		// 設定されていない
		if (path_from_settings.length() == 0) {
			return L"";
		}

		// 設定されていて、そのようなファイルが存在している
		if (PathFileExists(path_from_settings.c_str())) {
			return path_from_settings;
		}

		// 設定されていて、かつ、そのようなファイルは存在していない
		wstring errormsg = path_from_settings + L"というディレクトリは存在しません。\n" + xmlFileName + L"の" + wstrTargetKeyName + L"の値を正しいパスに設定してください。";
		MessageBox(NULL, errormsg.c_str(), L"パスエラー", NULL);
		return L"";
	}
}
