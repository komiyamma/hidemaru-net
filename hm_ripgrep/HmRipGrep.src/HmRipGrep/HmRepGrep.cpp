// Regex.Escape
// ���K�\�����܂񂾕������A�����̕�����ւƃG�X�P�[�v����


//�@�ȉ��́A�R�}���h���C�������Ƃ��ă_���ȕ�������G�X�P�[�v����

/// 
/// �R�}���h���C������ 1 ���G���R�[�h
/// 
public static string EncodeCommandLineValue(this string value)
{
	if (string.IsNullOrEmpty(value)) return "";
	var containsSpace = value.IndexOfAny(new[] {' ', '\t'}) != -1;

	// �u\�c\"�v���G�X�P�[�v
	// ����Ă邱�Ƃ́A�u"�v���O�́u\�v�̐��� 2�{+1
	value = _commandLineEscapePattern.Replace(value, @"$1\$&");

		// �X�y�[�X�^�^�u���܂܂��ꍇ�̓f���~�^�ň͂݁A�������u\�v�������ꍇ�A�G�X�P�[�v
		if (containsSpace) {
			value = "\"" + _lastBackSlashPattern.Replace(value, "$1$1") + "\"";
		}
	return value;
}
private static readonly Regex _commandLineEscapePattern = new Regex("(\\\\*)\"");
private static readonly Regex _lastBackSlashPattern = new Regex(@"(\\ + )$");

/// 
/// �R�}���h���C�������������G���R�[�h���āA�X�y�[�X�Ō���
/// 
public static string EncodeCommandLineValues(this IEnumerable<string> values)
{
	if (values == null) throw new ArgumentNullException("values");
	return string.Join(" ", values.Select(v = > EncodeCommandLineValue(v)));
}