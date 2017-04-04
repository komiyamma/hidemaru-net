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

#ifndef LIBNMH_H
#define LIBNMH_H

#ifdef __cplusplus
extern "C" {
#endif

	enum NMH_CHAR_CODE {
		NMH_CHAR_CODE_JIS = 0,
		NMH_CHAR_CODE_UTF8 = 1,
		NMH_CHAR_CODE_UTF8N = 2,
		NMH_CHAR_CODE_UTF16BE = 3,
		NMH_CHAR_CODE_UTF16LE = 4,
		NMH_CHAR_CODE_UTF32BE = 5,
		NMH_CHAR_CODE_UTF32LE = 6,
		NMH_CHAR_CODE_SJIS = 7,
		NMH_CHAR_CODE_EUC = 8,
		NMH_CHAR_CODE_ASCII = 9,
		NMH_CHAR_CODE_UNKNOWN = 10
	};

	extern const char *NMH_CHAR_CODE_NAME_LIST[];

	float nmh_is_jis(const unsigned char *str, int size);
	float nmh_is_utf8(const unsigned char *str, int size);
	float nmh_is_utf8n(const unsigned char *str, int size);
	float nmh_is_utf16(const unsigned char *str, int size);
	float nmh_is_utf32(const unsigned char *str, int size);
	float nmh_is_ascii(const unsigned char *str, int size);
	float nmh_is_euc(const unsigned char *str, int size);
	float nmh_is_sjis(const unsigned char *str, int size);
	enum NMH_CHAR_CODE nmh_code_detect(const char *str, int size);

#ifdef __cplusplus
}
#endif

#endif /* LIBNMH_H */

