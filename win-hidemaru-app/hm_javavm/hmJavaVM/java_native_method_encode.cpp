#include "java_native_method_encode.h"


TEncodingParam _GetEncodingParamFromHmEncode(const int hm_encode) {
	/*
	0,      // Unknown
	932,    // encode = 1 ANSI/OEM Japanese; Japanese (Shift-JIS)
	1200,   // encode = 2 Unicode UTF-16, little-endian
	51932,  // encode = 3 EUC
	50221,  // encode = 4 JIS
	65000,  // encode = 5 UTF-7
	65001,  // encode = 6 UTF-8
	1201,   // encode = 7 Unicode UTF-16, big-endian
	1252,   // encode = 8 欧文 ANSI Latin 1; Western European (Windows)
	936,    // encode = 9 簡体字中国語 ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
	950,    // encode =10 繁体字中国語 ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
	949,    // encode =11 韓国語 ANSI/OEM Korean (Unified Hangul Code)
	1361,   // encode =12 韓国語 Korean (Johab)
	1250,   // encode =13 中央ヨーロッパ言語 ANSI Central European; Central European (Windows)
	1257,   // encode =14 バルト語 ANSI Baltic; Baltic (Windows)
	1253,   // encode =15 ギリシャ語 ANSI Greek; Greek (Windows)
	1251,   // encode =16 キリル言語 ANSI Cyrillic; Cyrillic (Windows)
	42,     // encode =17 シンボル
	1254,   // encode =18 トルコ語 ANSI Turkish; Turkish (Windows)
	1255,   // encode =19 ヘブライ語 ANSI Hebrew; Hebrew (Windows)
	1256,   // encode =20 アラビア語 ANSI Arabic; Arabic (Windows)
	874,    // encode =21 タイ語 ANSI/OEM Thai (same as 28605, ISO 8859-15); Thai (Windows)
	1258,   // encode =22 ベトナム語 ANSI/OEM Vietnamese; Vietnamese (Windows)
	10001,  // encode =23 x-mac-japanese Japanese (Mac)
	850,    // encode =24 OEM/DOS
	0,      // encode =25 その他
	12000,  // encode =26 Unicode (UTF-32) little-endian
	12001,  // encode =27 Unicode (UTF-32) big-endian
	*/
	TEncodingParam param;

	if (hm_encode <= 0) {
		param.hm_encode = hm_encode;
		param.ms_codepage = 0;
		param.java_encoding_alias = L"";

		return param;
	}

	static const int code_arr[] = {
		0,
		932,
		1200,
		51932,
		50221,
		65000,
		65001,
		1201,
		1252,
		936,
		950,
		949,
		1361,
		1250,
		1257,
		1253,
		1251,
		42,
		1254,
		1255,
		1256,
		874,
		1258,
		10001,
		850,
		0,
		12000,
		12001
	};

	// コードページの一覧表の個数
	const int code_arr_length = sizeof(code_arr) / sizeof(int);

	// 範囲チェック(秀丸が返したencode値が個数をはみ出していたら...)
	if (hm_encode >= code_arr_length) {
		param.hm_encode = hm_encode;
		param.ms_codepage = 0;
		param.java_encoding_alias = L"";

		return param;
	}
	else {
		param.hm_encode = hm_encode;
		param.ms_codepage = code_arr[hm_encode];
	}



	switch (param.ms_codepage) {
	case 0: {
		param.java_encoding_alias = L"";
		break;
	}
	case 932: {
		param.java_encoding_alias = L"MS932";
		break;
	}
	case 1200: {
		param.java_encoding_alias = L"UTF-16LE";
		break;
	}
	case 51932: {
		param.java_encoding_alias = L"EUC_JP";
		break;
	}
	case 50221: {
		param.java_encoding_alias = L"Cp50221";
		break;
	}
	case 65000: {
		param.java_encoding_alias = L"UTF7";
		break;
	}
	case 65001: {
		param.java_encoding_alias = L"UTF8";
		break;
	}
	case 1201: {
		param.java_encoding_alias = L"UTF-16BE";
		break;
	}
	case 1252: {
		param.java_encoding_alias = L"Cp1252";
		break;
	}
	case 936: {
		param.java_encoding_alias = L"GB2312";
		break;
	}
	case 950: {
		param.java_encoding_alias = L"Big5";
		break;
	}
	case 949: {
		param.java_encoding_alias = L"Cp949";
		break;
	}
	case 1361: {
		param.java_encoding_alias = L"x-Johab";
		break;
	}
	case 1250: {
		param.java_encoding_alias = L"Cp1250";
		break;
	}
	case 1257: {
		param.java_encoding_alias = L"Cp1257";
		break;
	}
	case 1253: {
		param.java_encoding_alias = L"Cp1253";
		break;
	}
	case 1251: {
		param.java_encoding_alias = L"Cp1251";
		break;
	}
	case 42: {
		param.java_encoding_alias = L"Symbol";
		break;
	}
	case 1254: {
		param.java_encoding_alias = L"Cp1254";
		break;
	}
	case 1255: {
		param.java_encoding_alias = L"Cp1255";
		break;
	}
	case 1256: {
		param.java_encoding_alias = L"Cp1256";
		break;
	}
	case 874: {
		param.java_encoding_alias = L"MS874";
		break;
	}
	case 1258: {
		param.java_encoding_alias = L"Cp1258";
		break;
	}
	case 10001: {
		param.java_encoding_alias = L"x-MacJapanese";
		break;
	}
	case 850: {
		param.java_encoding_alias = L"Cp850";
		break;
	}
	case 12000: {
		param.java_encoding_alias = L"UTF-32LE";
		break;
	}
	case 12001: {
		param.java_encoding_alias = L"UTF-32BE";
		break;
	}
	}

	return param;
}