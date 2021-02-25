
#include <windows.h>
#include <string>
#include <vector>


using namespace std;

extern unsigned int encode_map[];

// この結果のバイト列(vector.data())を HmOutputPane.dllのOutput関数になげれば、Unicodeでも再現できる
vector<BYTE> EncodeWStringToOriginalEncodeVector(wstring original_string) {
	vector<BYTE> r;
	for (wchar_t ch : original_string) {
		int ix = ch;

		// 変換後のコードの数値が、255以下なら
		unsigned int encode_code = encode_map[ix];
		if (encode_code <= 0xFF) { // １文字で収まる
			r.push_back(encode_code);
		}
		else {
			union EncodeCodeUnion {
				unsigned int code;
				BYTE ch[4];
			};

			EncodeCodeUnion ecu;
			ecu.code = encode_code;
			for (BYTE b : ecu.ch) {
				if (b == 0) {
					break;
				}
				r.push_back(b);
			}
		}
	}

	r.push_back(0);
	return r;
}

