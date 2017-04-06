
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <map>

#include "OnigRegex.h"
#include "OutputDebugStream.h"

using namespace std;


// 正規表現マッチング用の関数
// @str_target

int OnigMatch( string str_target, string str_regex, Matches* pmatches) {

	// まず、検索結果は削除
	if ( pmatches ) {
		pmatches->clear();
	}

	// どちらかが中身からなら、falseを返す。
	if ( str_target.length() == 0 || str_regex.length() == 0 ) {
		return false;
	}

	int r;
	OnigUChar *start, *range, *end;
	regex_t* reg;
	OnigErrorInfo einfo;
	OnigRegion *region;

	UChar* pattern = (UChar* )str_regex.c_str(); // "あ(.+?)(うえ)お|[あ-う]+";
	UChar* str     = (UChar* )str_target.c_str(); // "きあ\x0Aいう\x0Aえおか";

	// 正規表現の作成。 SJIS
	r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
		ONIG_OPTION_IGNORECASE|ONIG_OPTION_MULTILINE, ONIG_ENCODING_SJIS, ONIG_SYNTAX_DEFAULT, &einfo);

	if (r != ONIG_NORMAL) {
		char s[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str((OnigUChar *)s, r, &einfo);
		OutputDebugStream("正規表現コンパイルエラー: %s\n", s);
		return false;
	}

	region = onig_region_new();

	end   = str + strlen((char* )str);
	start = str;
	range = end;

	// 正規表現で検索する。
	r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
	if (r >= 0) {
		int i;

		// マッチした部分全体と、( )の中身の表示。
		// fprintf(stderr, "match at %d\n", r);
		for (i = 0; i < region->num_regs; i++) {

			if ( pmatches ) {
				int len = region->end[i]-region->beg[i];
				// 文字をコピーするための領域を動的に確保
				char *cp = new char[len+1]; // 最初と最後の差分を確保。endが１つ多めなので、これでNULL文字分も確保されている。

				// 文字列をコピー
				// 　　　　　　┌コピー開始位置      ┌コピーバイト数
				strncpy( cp, (char *)(str)+region->beg[i], len );
				cp[len] = '\0'; // 最後にNULL文字を入れる。

				// OutputDebugStream("%s", cp);
				pmatches->insert( Matches::value_type( i, cp ) );

				// 動的な確保領域解放
				delete [] cp;
			}
		}
	}
	else if (r == ONIG_MISMATCH) {
		// OutputDebugStream("search fail\n");
	}
	else { /* error */
		char s[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str((OnigUChar *)s, r);
		// OutputDebugStream("正規表現マッチ実行エラー: %s\n", s);
		return false;
	}

	onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
	onig_free(reg);

	if ( r >= 0 ) {
		return true;
	} else {
		return false;
	}
}

