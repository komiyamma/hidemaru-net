use Regex::PreSuf;

my $HIDE_LINE_CHAR_LIMIT = 240;

if (@ARGV >= 1 && -e $ARGV[0]) {
	
	if (@ARGV >= 2 && $ARGV[1] =~ /^\d+$/) {
		$HIDE_LINE_CHAR_LIMIT = $ARGV[1];
	}
	
	# ファイルから読み込み配列へ。
	open FH, $ARGV[0] or die "引数のファイルが不正なようです。";
	my @lines = <FH>;
	close FH;
	
	# 1行ずつ読み込んで、それまでの単語も足して、圧縮した結果が $HIDE_LINE_CHAR_LIMIT未満なら、足しこんでいく
	my $iCurBufLength = 0;
	my @listCurBuf = ();
	for my $line (@lines) {
		chomp($line);
		# 実験で次の単語を足してみる
		my @nextCurBuf = @listCurBuf;
		push @nextCurBuf, $line;
		
		# 正規表現圧縮してみる
		my $reNext = presuf(@nextCurBuf);
				
		# はみ出したら、出力
		if (length($reNext) > $HIDE_LINE_CHAR_LIMIT) {
			PrintRegex(@listCurBuf);
			
			# 初期化
			$iCurBufLength = 0;
			@listCurBuf = ();

		}
		
		# 今回の$lineをBufへと加える			
		$iCurBufLength = $iCurBufLength + length($line);
		push @listCurBuf, $line;
		
	}
	
	# 最後残りがあれば出す
	if ($iCurBufLength > 0) {
		PrintRegex(@listCurBuf);
	}	
}

sub PrintRegex {
	my $reCurr = presuf(@_);
	$reCurr =~ s/\(\?\:/\(/g;
	print($reCurr, "\n");
}