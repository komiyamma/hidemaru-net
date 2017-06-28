#include "java_vm_engine.h"
#include "self_dll_info.h"
#include "convert_string.h"
#include "output_debugstream.h"

#include <shlwapi.h>
#include <iostream>
#include <string>

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

	wstring xmlpath = CSelfDllInfo::GetSelfModuleDir() + L"\\hmJavaXM.xml";

	if (PathFileExists(xmlpath.c_str())) {
		string cp932_xmlpath = utf16_to_cp932(xmlpath);
		try {
			TiXmlDocument doc(cp932_xmlpath.c_str());
			doc.LoadFile();

			auto nodeConfiguration = doc.FirstChildElement("configuration");
			if (!nodeConfiguration) {
				MessageBox(NULL, L"XMLの構成に不具合1。", L"XMLの構成に不具合。", NULL);
				return L"";
			}

			auto nodeAppSettings = nodeConfiguration->FirstChildElement("appSettings");
			if (!nodeAppSettings) {
				MessageBox(NULL, L"XMLの構成に不具合2。", L"XMLの構成に不具合。", NULL);
				return L"";
			}

			TiXmlElement *elementAdd = nodeAppSettings->FirstChildElement("add");
			while(true) {
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
		return LR"PATH(C:\Program Files (x86)\Java\jdk1.8.0_131\jre\bin\client\jvm.dll)PATH";

	}
	else {
		return L"";
	}
}

