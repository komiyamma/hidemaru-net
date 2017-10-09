#include "self_dll_info.h"
#include "convert_string.h"

#include <shlwapi.h>
#include "tinyxml.h"

#include "patchlevel.h"

/*
TiXmlDocument::LoadFile�c�t�@�C����ǂݍ���
�ETiXmlDocument::RootElement�cXML�؍\���̃��[�g�̗v�f���擾����
�ETiXmlElement::FirstChildElement�c�w�肵���v�f�����q�̗v�f��1�ڂ��擾����
�ETiXmlElement::NextSiblingElement�cFirstChildElement�̌�Ɏg�����ƂŁC���̗v�f���擾����
�ETiXmlElement::Value�c�w�肵���v�f����<tag a="str">777</tag>��tag���擾����
�ETiXmlElement::GetText�c�w�肵���v�f����<tag a="str">777</tag>��777���擾����
�ETiXmlElement::Attribute�c�w�肵���v�f����<tag a="str">777</tag>��a���w�肵���ꍇ�Cstr���擾����
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
				MessageBox(NULL, L"XML��configuration�\���ɕs�������܂��B", L"XML��configuration�\���ɕs�������܂��B", NULL);
				return L"";
			}

			auto nodeAppSettings = nodeConfiguration->FirstChildElement("appSettings");
			if (!nodeAppSettings) {
				MessageBox(NULL, L"XML��appSettings�\���ɕs�������܂��B", L"XML��appSettings�\���ɕs�������܂��B", NULL);
				return L"";
			}

			TiXmlElement *elementAdd = nodeAppSettings->FirstChildElement("add");
			if (!elementAdd) {
				MessageBox(NULL, L"XML��add�\���ɕs�������܂��B", L"XML��add�\���ɕs�������܂��B", NULL);
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

	// XML�t�@�C���͑��݂��Ȃ�
	else {
		wstring defaultAbsolutePath = CSelfDllInfo::GetSelfModuleDir() + defaultRelativePath;
		// �f�t�H���g�ŃC���X�g�[�����Ă���HmPython�t�H���_������A���̒��ɁA
		if ( PathFileExists(defaultAbsolutePath.c_str()) ) {
			return defaultAbsolutePath;
		}

		return L"";
	}

}

namespace PythonEngine {

	wstring GetPythonPath() {

		// XML�ɐݒ肳��Ă���PYTHONPATH�̒l
		wstring path_from_settings = L"";
		
		try {
			path_from_settings = LoadPythonSettingsPathFromXml();
		}
		catch (exception e) {

		}

		// �ݒ肳��Ă��Ȃ�
		if (path_from_settings.length() == 0) {
			return L"";
		}

		// �ݒ肳��Ă��āA���̂悤�ȃt�@�C�������݂��Ă���
		if (PathFileExists(path_from_settings.c_str())) {
			return path_from_settings;
		}

		// �ݒ肳��Ă��āA���A���̂悤�ȃt�@�C���͑��݂��Ă��Ȃ�
		wstring errormsg = path_from_settings + L"�Ƃ����f�B���N�g���͑��݂��܂���B\n" + xmlFileName + L"��" + wstrTargetKeyName + L"�̒l�𐳂����p�X�ɐݒ肵�Ă��������B";
		MessageBox(NULL, errormsg.c_str(), L"�p�X�G���[", NULL);
		return L"";
	}
}
