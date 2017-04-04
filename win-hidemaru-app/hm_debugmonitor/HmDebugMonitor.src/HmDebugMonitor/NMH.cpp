/*
* WRITTEN BY Masahiko Higashiyama in 2010.
*
* THIS CODE IS IN PUBLIC DOMAIN.
* THIS SOFTWARE IS COMPLETELY FREE TO COPY, MODIFY AND/OR RE-DISTRIBUTE.
*
* THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "nmh.h"

const char *NMH_CHAR_CODE_NAME_LIST[] = {
	"JIS",
	"UTF8",
	"UTF8BOM",
	"UTF16BE",
	"UTF16LE",
	"UTF32BE",
	"UTF32LE",
	"CP932",
	"EUC",
	"ASCII",
	"UNKNOWN"
};


float nmh_is_utf16(const unsigned char *str, int size){
	if (size < 2) return -1.0;
	if (str[0] == 0xFE && str[1] == 0xFF){
		// str is UTF-16BE
		return 1.5;
	}
	else if (str[0] == 0xFF && str[1] == 0xFE){
		// str is UTF-16LE
		return 0.5;
	}
	return -1.0;
}

float nmh_is_utf32(const unsigned char *str, int size){
	if (size < 4) return -1.0;
	if (str[0] == 0x00 && str[1] == 0x00 &&
		str[3] == 0xFE && str[4] == 0xFF){
		// str is UTF-32BE
		return 1.5;
	}
	else if (str[0] == 0xFF && str[1] == 0xFE &&
		str[2] == 0x00 && str[3] == 0x00){
		// str is UTF-32LE
		return 0.5;
	}
	return -1.0;
}

float nmh_is_jis(const unsigned char *str, int size){
	int i;
	const unsigned char *p = str;
	unsigned char c1, c2;
	for (i = 0; i < size - 1; i++) {
		c1 = *p;
		if (c1 == 0x00 || c1 > 0x7F) return -1.0;
		if (c1 == 0x1B){
			c2 = *(p + 1);
			if (c2 == 0x24 || c2 == 0x28) return 1.0;
		}
		p++;
	}
	return -1.0;
}

float nmh_is_utf8n(const unsigned char *str, int size){
	if (size < 3) return -1.0;
	if (*str == 0xEF && *(str + 1) == 0xBB && *(str + 2) == 0xBF)
		return 1.0;
	return -1.0;
}

static int nmh_utf8charlen(const unsigned char c)
{
	if (c == 0x00) return -1;
	if (c < 0x80) return 1;
	if (c < 0xC2) return -1;
	if (c < 0xE0) return 2;
	if (c < 0xF0) return 3;
	if (c < 0xF8) return 4;
	if (c < 0xFC) return 5;
	if (c < 0xFE) return 6;
	return -1;
}

float nmh_is_utf8(const unsigned char *str, int size){
	int i, j;
	int charlen;
	const unsigned char *p = str;
	float isutf8n = -1.0;

	if ((isutf8n = nmh_is_utf8n(str, size)) > 0.0)
		str += 3;

	for (i = 0; i < size; i++){
		if ((charlen = nmh_utf8charlen(*p)) == -1)
			return -1.0;
		p++;
		for (j = 0; j < charlen - 1; j++, i++, p++){
			if (*p == 0x00 || (*p & 0xC0) != 0x80) return -1.0;
		}
	}
	return (float)(isutf8n > 0.0 ? 1.5 : 0.5);
}

float nmh_is_ascii(const unsigned char *str, int size){
	const unsigned char *p = str;
	int i = 0;
	for (i = 0; i < size; i++, p++){
		if (*p == 0x00 || *p > 0x7F){
			return -1.0;
		}
	}
	return 1.0;
}

float nmh_is_euc(const unsigned char *str, int size){
	int i;
	const unsigned char *p = str;
	unsigned char c1, c2, c3;
	float score = 1.0;

	for (i = 0; i < size; i++, p++){
		c1 = *p;
		if (c1 == 0x00){
			return -1.0;
		}
		else if (c1 <= 0x7F){
			// ASCII
			continue;
		}
		else if (c1 >= 0xA1 && c1 <= 0xFE && i < size - 1){
			// kanji
			c2 = *(p + 1);
			if (c2 >= 0xA1 && c2 <= 0xFE){
				i++; p++;
			}
			else{
				return -1.0;
			}
		}
		else if (c1 == 0x8E && i < size - 1){
			// kana
			c2 = *(p + 1);
			if (c2 >= 0xA1 && c2 <= 0xDF){
				i++; p++; score *= 0.9f;
			}
			else{
				return -1.0;
			}
		}
		else if (c1 == 0x8F && i < size - 2){
			// hojo kanji
			c2 = *(p + 1); c3 = *(p + 2);
			if ((c2 >= 0xA1 && c2 <= 0xFE)
				&& (c3 >= 0xA1 && c3 <= 0xFE)){
				i += 2; p += 2; score *= 0.9f;
			}
			else{
				return -1.0;
			}
		}
	}

	return score;
}

float nmh_is_sjis(const unsigned char *str, int size){
	int i;
	const unsigned char *p = str;
	unsigned char c1, c2;
	float score = 1.0;

	for (i = 0; i < size; i++, p++){
		c1 = *p;
		if (c1 == 0x00){
			return -1.0;
		}
		else if (c1 <= 0x7F){
			// ASCII
			continue;
		}
		else if (c1 >= 0xA1 && c1 <= 0xDF){
			// kana
			score *= 0.9f;
			continue;
		}
		else if (((c1 >= 0x81 && c1 <= 0x9F) ||
			(c1 >= 0xE0 && c1 <= 0xFC))
			&& i < size - 1){
			// kanji
			c2 = *(p + 1);
			if ((c2 >= 0x40 && c2 <= 0x7E) ||
				(c2 >= 0x80 && c2 <= 0xFC)){
				i++; p++;
			}
			else{
				return -1.0;
			}
		}
		else{
			return -1.0;
		}
	}
	return score;
}

enum NMH_CHAR_CODE
	nmh_code_detect(const char *_str, int size){
	float isutf8, issjis, iseuc, isutf16, isutf32;
	const unsigned char *str = (const unsigned char*)_str;

	if ((isutf32 = nmh_is_utf32(str, size)) > 0.0){
		if (isutf32 > 1.0)
			return NMH_CHAR_CODE_UTF32BE;
		else
			return NMH_CHAR_CODE_UTF32LE;
	}
	if ((isutf16 = nmh_is_utf16(str, size)) > 0.0){
		if (isutf16 > 1.0)
			return NMH_CHAR_CODE_UTF16BE;
		else
			return NMH_CHAR_CODE_UTF16LE;
	}

	if (nmh_is_jis(str, size) > 0.0) return NMH_CHAR_CODE_JIS;
	if (nmh_is_ascii(str, size) > 0.0) return NMH_CHAR_CODE_ASCII;

	isutf8 = nmh_is_utf8(str, size);
	if (isutf8 > 1.0) return NMH_CHAR_CODE_UTF8N;
	if (isutf8 > 0.0) return NMH_CHAR_CODE_UTF8;

	issjis = nmh_is_sjis(str, size);
	iseuc = nmh_is_euc(str, size);
	if (issjis > 0.0 && iseuc > 0.0)
		return issjis >= iseuc ? NMH_CHAR_CODE_SJIS : NMH_CHAR_CODE_EUC;
	if (issjis > 0.0) return NMH_CHAR_CODE_SJIS;
	if (iseuc > 0.0) return NMH_CHAR_CODE_EUC;


	return NMH_CHAR_CODE_UNKNOWN;
}


