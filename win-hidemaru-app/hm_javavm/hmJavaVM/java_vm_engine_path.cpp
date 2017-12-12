/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#include "dllfunc_interface.h"
#include "java_vm_engine.h"
#include "self_dll_info.h"
#include "convert_string.h"
#include "output_debugstream.h"

#include <shlwapi.h>
#include <iostream>
#include <string>
#include <shlobj.h>
#include "tinyxml.h"

/*
TiXmlDocument::LoadFile…ファイルを読み込む
• TiXmlDocument::RootElement…XML木構造のルートの要素を取得する
• TiXmlElement::FirstChildElement…指定した要素が持つ子の要素の1つ目を取得する
• TiXmlElement::NextSiblingElement…FirstChildElementの後に使うことで，次の要素を取得する
• TiXmlElement::Value…指定した要素から<tag a="str">777</tag>のtagを取得する
• TiXmlElement::GetText…指定した要素から<tag a="str">777</tag>の777を取得する
• TiXmlElement::Attribute…指定した要素から<tag a="str">777</tag>のaを指定した場合，strを取得する
*/
wstring CJavaVMEngine::GetHavaVMDllPath() {
	wstring path_from_settings = GetHavaVMDllPathFromSettingsFile();
	if ( PathFileExists(path_from_settings.c_str() ) ) {
		return path_from_settings;
	}
	wstring errormsg = path_from_settings + L"というファイルは存在しません。\nhmJavaVM.xmlの「JavaVM_PATH」の値を正しいパスに設定してください。";
	MessageBox(NULL, errormsg.c_str(), L"パスエラー", NULL);
	return L"";
}

wstring CJavaVMEngine::GetHavaVMDllPathFromSettingsFile() {
	wstring xmlpath = CSelfDllInfo::GetSelfModuleDir() + L"\\hmJavaVM.xml";

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
					if (string(key) == "JavaVM_PATH") {
						return cp932_to_utf16(value);
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
	else {
		return L"";
	}

}
