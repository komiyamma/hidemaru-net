/* 
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#define MACRO_DLL extern "C" __declspec(dllexport)

#include <map>
#include "sol.hpp"
#include "OutputDebugStream.h"
#include <windows.h>

#include "string_converter.h"

using namespace std;

// �p�j�b�N�ɂȂ������悤�ɌĂ΂��֐�
int atpanic(lua_State *L)
{
	throw std::runtime_error(lua_tostring(L, -1));
}
int le_trace(lua_State *L)
{
	int n = lua_gettop(L);
	for (int i = 1; i <= n; i++) {
		lua_getglobal(L, "tostring");
		lua_pushvalue(L, i);
		lua_pcall(L, 1, 1, 0);
		const char *str = lua_tostring(L, -1);
		lua_OutputDebugStream(str);
		lua_pop(L, 1);
	}
	lua_OutputDebugStream("\n");
	return 0;
}

extern "C" int codepage = 932;

// create an empty lua state
sol::state lua;

void LoadLuaInterpreter() {

	codepage = 932;

	lua.open_libraries(sol::lib::base);
	lua.open_libraries(sol::lib::package);
	lua.open_libraries(sol::lib::coroutine);
	lua.open_libraries(sol::lib::string);
	lua.open_libraries(sol::lib::table);
	lua.open_libraries(sol::lib::math);
	lua.open_libraries(sol::lib::bit);
	lua.open_libraries(sol::lib::bit32);
	lua.open_libraries(sol::lib::io);
	lua.open_libraries(sol::lib::os);
	lua.open_libraries(sol::lib::debug);
	lua.open_libraries(sol::lib::ffi);
	lua.open_libraries(sol::lib::jit);
	lua.open_libraries(sol::lib::lfs);
	lua.open_libraries(sol::lib::utf8);
	lua.open_libraries(sol::lib::cp932);
	lua.open_libraries(sol::lib::cjson);

	// �p�j�b�N�ɂȂ����炱����Ă�ł�
	lua_atpanic(lua.lua_state(), atpanic);
}



void FreeLuaInterpreter() {
	string expression = R"(
		if DestroyScope and type(DestroyScope) == "function" then
			DestroyScope()
        end
	)";

	lua.script(expression);

	codepage = 932;
}

// �u932�v���u65001�v��ݒ�
MACRO_DLL int SetCodePage(int cp) {
	codepage = cp;
	return TRUE;
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _GetNumVar(const string& var_name) {
		try {
			sol::object value = lua[var_name];
			return value.as<int>();
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return 0;
		}
	}
}

MACRO_DLL int GetNumVar(const void *sz_var_name) {
	if (codepage == 65001) {
		string var_name = utf16_to_utf8((wchar_t *)sz_var_name);
		return raw::_GetNumVar(var_name);
	}
	else {
		return raw::_GetNumVar((char *)sz_var_name);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _SetNumVar(const string& var_name, int value) {
		try {
			lua[var_name] = value;
			return TRUE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}


MACRO_DLL int SetNumVar(const void *sz_var_name, int value) {

	if (codepage == 65001) {
		string var_name = utf16_to_utf8((wchar_t *)sz_var_name);
		return raw::_SetNumVar(var_name, value);
	}
	else {
		return raw::_SetNumVar((char *)sz_var_name, value);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	const string _GetStrVar(const string &var_name) {
		try {
			sol::object value = lua[var_name];
			// �ۑ����Ă�����̃|�C���^��Ԃ��K�v������B
			return value.as<std::string>();
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return "";
		}
	}
}

MACRO_DLL const void* GetStrVar(const void *sz_var_name) {
	if (codepage == 65001) {
		string var_name = utf16_to_utf8((wchar_t *)sz_var_name);
		static wstring utf16 = L"";
		string utf8 = raw::_GetStrVar(var_name);
		utf16 = utf8_to_utf16(utf8);
		return utf16.data();
	}
	else {
		static string sjis = "";
		sjis = raw::_GetStrVar((char *)sz_var_name);
		return sjis.data();
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _SetStrVar(const string &var_name, const string &value) {
		try {
			lua[var_name] = value;
			return TRUE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}

MACRO_DLL int SetStrVar(const void *sz_var_name, const void *sz_value) {
	if (codepage == 65001) {
		string var_name = utf16_to_utf8((wchar_t *)sz_var_name);
		string value = utf16_to_utf8((wchar_t *)sz_value);
		return raw::_SetStrVar(var_name, value);
	}
	else {
		return raw::_SetStrVar((char *)sz_var_name, (char *)sz_value);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _GetNumItemOfList(const string& arr_name, int ix) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				sol::object value = lua[arr_name][ix];
				return value.as<int>();
			}
			return 0;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return 0;
		}
	}
}

MACRO_DLL int GetNumItemOfList(const void *sz_arr_name, int ix) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		return raw::_GetNumItemOfList(arr_name, ix);
	}
	else {
		return raw::_GetNumItemOfList((char *)sz_arr_name, ix);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _SetNumItemOfList(const string& arr_name, int ix, int value) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				lua[arr_name][ix] = value;
				return TRUE;
			}
			return FALSE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}
MACRO_DLL int SetNumItemOfList(const void *sz_arr_name, int ix, int value) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		return raw::_SetNumItemOfList(arr_name, ix, value);
	}
	else {
		return raw::_SetNumItemOfList((char *)sz_arr_name, ix, value);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	const string _GetStrItemOfList(const string arr_name, int ix) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				sol::object value = lua[arr_name][ix];
				// �ۑ����Ă�����̃|�C���^��Ԃ��K�v������B
				return value.as<std::string>();
			}
			return "";
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return "";
		}
	}
}

MACRO_DLL const void* GetStrItemOfList(const void *sz_arr_name, int ix) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		static wstring utf16 = L"";
		string utf8 = raw::_GetStrItemOfList(arr_name, ix);
		utf16 = utf8_to_utf16(utf8);
		return utf16.data();
	}
	else {
		static string sjis = "";
		sjis = raw::_GetStrItemOfList((char *)sz_arr_name, ix);
		return sjis.data();
	}
}


//--------------------------------------------------------------------------------------
// ��
namespace raw {

	int _SetStrItemOfList(const string &arr_name, int ix, const string &value) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				lua[arr_name][ix] = string(value);
				return TRUE;
			}
			return FALSE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}

MACRO_DLL int SetStrItemOfList(const void *sz_arr_name, int ix, const void *sz_value) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		string value = utf16_to_utf8((wchar_t *)sz_value);
		return raw::_SetStrItemOfList(arr_name, ix, value);
	}
	else {
		return raw::_SetStrItemOfList((char *)sz_arr_name, ix, (char *)sz_value);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _GetNumItemOfDict(const string &arr_name, const string &key_name) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				sol::object value = lua[arr_name][key_name];
				return value.as<int>();
			}
			return 0;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return 0;
		}
	}
}
MACRO_DLL int GetNumItemOfDict(const void *sz_arr_name, const void *sz_key_name) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		string key_name = utf16_to_utf8((wchar_t *)sz_key_name);
		return raw::_GetNumItemOfDict(arr_name, key_name);
	}
	else {
		return raw::_GetNumItemOfDict((char *)sz_arr_name, (char *)sz_key_name);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _SetNumItemOfDict(const string &arr_name, const string &key_name, int value) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				lua[arr_name][key_name] = value;
				return TRUE;
			}
			return FALSE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}

MACRO_DLL int SetNumItemOfDict(const void *sz_arr_name, const void *sz_key_name, int value) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		string key_name = utf16_to_utf8((wchar_t *)sz_key_name);
		return raw::_SetNumItemOfDict(arr_name, key_name, value);
	}
	else {
		return raw::_SetNumItemOfDict((char *)sz_arr_name, (char *)sz_key_name, value);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	const string _GetStrItemOfDict(const string &arr_name, const string &key_name) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				sol::object value = lua[arr_name][key_name];
				// �ۑ����Ă�����̃|�C���^��Ԃ��K�v������B
				return value.as<std::string>();
			}
			return "";
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return "";
		}
	}
}

MACRO_DLL const void * GetStrItemOfDict(const void *sz_arr_name, const void * sz_key_name) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		string key_name = utf16_to_utf8((wchar_t *)sz_key_name);
		static wstring utf16 = L"";
		string utf8 = raw::_GetStrItemOfDict(arr_name, key_name);
		utf16 = utf8_to_utf16(utf8);
		return utf16.data();
	}
	else {
		static string sjis = "";
		sjis = raw::_GetStrItemOfDict((char *)sz_arr_name, (char *)sz_key_name);
		return sjis.data();
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _SetStrItemOfDict(const string &arr_name, const string &key_name, const string &value) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();
			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				lua[arr_name][key_name] = value;
				return TRUE;
			}
			return FALSE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}

}
MACRO_DLL int SetStrItemOfDict(const void*sz_arr_name, const void* sz_key_name, const void *sz_value) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		string key_name = utf16_to_utf8((wchar_t *)sz_key_name);
		string value = utf16_to_utf8((wchar_t *)sz_value);
		return raw::_SetStrItemOfDict(arr_name, key_name, value);
	}
	else {
		return raw::_SetStrItemOfDict((char *)sz_arr_name, (char *)sz_key_name, (char *)sz_value);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {

	int _GetLenOfList(const string& arr_name) {
		try {
			sol::object tbl = lua[arr_name];
			sol::type t = tbl.get_type();

			// �{���Ƀe�[�u���ł���΁c
			if (t == sol::type::table) {
				sol::table soltbl = tbl;
				return soltbl.size();
			}
			return 0;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return 0;
		}
	}
}

MACRO_DLL int GetLenOfList(const void *sz_arr_name) {
	if (codepage == 65001) {
		string arr_name = utf16_to_utf8((wchar_t *)sz_arr_name);
		return raw::_GetLenOfList(arr_name);
	}
	else {
		return raw::_GetLenOfList((char *)sz_arr_name);
	}
}

//--------------------------------------------------------------------------------------
// ��
namespace raw {
	int _DoString(const string& expression) {

		try {
			lua.script(expression);
			return TRUE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}

MACRO_DLL int DoString(const void *sz_expression) {

	if (codepage == 65001) {
		string expression = utf16_to_utf8((wchar_t *)sz_expression);
		return raw::_DoString(expression);
	}
	else {
		return raw::_DoString((char *)sz_expression);
	}
}
//--------------------------------------------------------------------------------------
namespace raw {
	int _DoFile(const string &filename) {
		try {
			lua.script_file(filename);
			return TRUE;
		}
		catch (std::exception& e) {
			lua_OutputDebugStream("%s\n", e.what());
			return FALSE;
		}
	}
}

MACRO_DLL int DoFile(const char *sz_filename) {

	if (codepage == 65001) {
		string filename = utf16_to_utf8((wchar_t *)sz_filename);
		return raw::_DoFile(filename);
	}
	else {
		return raw::_DoFile((char *)sz_filename);
	}
}