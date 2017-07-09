// Regex.Escape
// 正規表現を含んだ文字を、ただの文字列へとエスケープする


//　以下は、コマンドライン引数としてダメな文字列をエスケープする

/// 
/// コマンドライン引数 1 個をエンコード
/// 
public static string EncodeCommandLineValue(this string value)
{
	if (string.IsNullOrEmpty(value)) return "";
	var containsSpace = value.IndexOfAny(new[] {' ', '\t'}) != -1;

	// 「\…\"」をエスケープ
	// やってることは、「"」直前の「\」の数を 2倍+1
	value = _commandLineEscapePattern.Replace(value, @"$1\$&");

		// スペース／タブが含まれる場合はデリミタで囲み、末尾が「\」だった場合、エスケープ
		if (containsSpace) {
			value = "\"" + _lastBackSlashPattern.Replace(value, "$1$1") + "\"";
		}
	return value;
}
private static readonly Regex _commandLineEscapePattern = new Regex("(\\\\*)\"");
private static readonly Regex _lastBackSlashPattern = new Regex(@"(\\ + )$");

/// 
/// コマンドライン引数複数個をエンコードして、スペースで結合
/// 
public static string EncodeCommandLineValues(this IEnumerable<string> values)
{
	if (values == null) throw new ArgumentNullException("values");
	return string.Join(" ", values.Select(v = > EncodeCommandLineValue(v)));
}