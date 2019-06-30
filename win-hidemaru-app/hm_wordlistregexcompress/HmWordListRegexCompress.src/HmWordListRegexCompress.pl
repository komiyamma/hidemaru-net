#-----------------------------------------------------------------------------
# Copyright (C) 2016-2017 Akitsugu Komiyama
# under The MIT License
#-----------------------------------------------------------------------------

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



#-----------------------------------------------------------------------------
# Regex::Presuf
# This code is distributed under the same copyright terms as Perl itself.
# under The Artistic License 2.0
#    Copyright (C) Jarkko Hietaniemi
#    http://search.cpan.org/~jhi/Regex-PreSuf-1.17/PreSuf.pm
#-----------------------------------------------------------------------------

sub debug {
    if (@_) {
    $DEBUG = shift;
    } else {
    return $DEBUG;
    }
}

sub prefix_length {
    my $n = 0;
    my %diff;

    for(my $m = 0; ; $m++) {
    foreach (@_) {
            $diff{ @{$_} <= $m ? '' : $_->[$m] }++;
    }
        last if keys %diff > 1;
    if (exists $diff{ '' } and $diff{ '' } == @_) {
        %diff = ();
        last;
    }
    %diff = ();
        $n = $m+1;
    }

    return ($n, %diff);
}

sub suffix_length {
    my $n = 0;
    my %diff;

    for(my $m = 1; ; $m++) {
    foreach (@_){
        $diff{ @{$_} < $m ? '' : $_->[-$m] }++;
    }
        last if keys %diff > 1;
    if (exists $diff{ '' } and $diff{ '' } == @_) {
        %diff = ();
        last;
    }
        %diff = ();
        $n = $m;
    }

    return ($n, %diff);
}

sub _presuf {
    my $level = shift;
    my $INDENT = " " x $level if $DEBUG;
    my $param = shift;
    
    print "_presuf:$INDENT <- ", join(" ", map { join('', @$_) } @_), "\n"
    if $DEBUG;

    return '' if @_ == 0;

    if (@_ == 1) {
    my $presuf = join('', @{ $_[0] });
    print "_presuf:$INDENT -> $presuf\n" if $DEBUG;
    return $presuf;
    }

    my ($pre_n, %pre_d) = prefix_length @_;
    my ($suf_n, %suf_d) = suffix_length @_;

    if ($DEBUG) {
    print "_presuf:$INDENT pre_n = $pre_n (",join(" ",%pre_d),")\n";
    print "_presuf:$INDENT suf_n = $suf_n (",join(" ",%suf_d),")\n";
    }

    my $prefixes =  not exists $param->{ prefixes } ||
                           $param->{ prefixes };
    my $suffixes =             $param->{ suffixes } ||
               (    exists $param->{ prefixes } &&
                    not        $param->{ prefixes });

    if ($prefixes and not $suffixes) {
    # On qw(rattle rattlesnake) clear suffix.
    foreach (keys %pre_d) {
        if ($_ eq '') {
        $suf_n = 0;
        %suf_d = ();
        last;
        }
    }
    }

    if ($suffixes and not $prefixes) {
    foreach (keys %suf_d) {
        if ($_ eq '') {
        $pre_n = 0;
        %pre_d = ();
        last;
        }
    }
    }

    if ($pre_n or $suf_n) {
    if ($pre_n == $suf_n) {
        my $eq_n = 1;
        my $eq_s = join('', @{ $_[0] });

        foreach (@_[ 1 .. $#_ ]) {
        last if $eq_s ne join('', @{ $_ });
        $eq_n++;
        }

        if ($eq_n == @_) {  # All equal.  How boring.
        print "_presuf:$INDENT -> $eq_s\n" if $DEBUG;
        return $eq_s;
        }
    }

    my $ps_n = $pre_n + $suf_n;
    my $overlap; # Guard against prefix and suffix overlapping.

    foreach (@_) {
        if (@{ $_ } < $ps_n) {
        $overlap = 1;
        last;
        }
    }

    # Remove prefixes and suffixes and recurse.

    my $pre_s = $pre_n ?
                join('', @{ $_[0] }[ 0 .. $pre_n - 1 ]) : '';
    my $suf_s = $suf_n ?
                join('', @{ $_[0] }[ -$suf_n .. -1 ]) : '';
    my @presuf;

    if ($overlap) {
        if ($prefixes and not $suffixes) {
        $suf_s = '';
        foreach (@_) {
            push @presuf,
                         [ @{ $_ }[ $pre_n .. $#{ $_ } ] ];
        }
        } elsif ($suffixes) {
        $pre_s = '';
        foreach (@_) {
            push @presuf,
                         [ @{ $_ }[ 0 .. $#{ $_ } - $suf_n ] ];
        }
        }
    } else {
        foreach (@_) {
        push @presuf,
                     [ @{ $_ }[ $pre_n .. $#{ $_ } - $suf_n ] ];
        }
    }

    if ($DEBUG) {
        print "_presuf:$INDENT pre_s = $pre_s\n";
        print "_presuf:$INDENT suf_s = $suf_s\n";
        print "_presuf:$INDENT presuf = ",
              join(" ", map { join('', @$_) } @presuf), "\n";
    }

    my $presuf = $pre_s . _presuf($level + 1, $param, @presuf) . $suf_s;

    print "_presuf:$INDENT -> $presuf\n" if $DEBUG;

    return $presuf;
    } else {
    my @len_n;
    my @len_1;
    my $len_0 = 0;
    my (@alt_n, @alt_1);

    foreach (@_) {
        my $len = @{$_};
        if    ($len >  1) { push @len_n, $_ }
        elsif ($len == 1) { push @len_1, $_ }
        else              { $len_0++        } # $len == 0
    }

    # NOTE: does not preserve the order of the words.

    if (@len_n) {    # Alternation.
        if (@len_n == 1) {
        @alt_n = join('', @{ $len_n[0] });
        } else {
        my @pre_d = keys %pre_d;
        my @suf_d = keys %suf_d;

        my (%len_m, @len_m);

        if ($prefixes and $suffixes) {
            if (@pre_d < @suf_d) {
            $suffixes = 0;
            } else {
            if (@pre_d == @suf_d) {
                if ( $param->{ suffixes } ) {
                $prefixes = 0;
                } else {
                $suffixes = 0;
                }
            } else {
                $prefixes = 0;
            }
            }
        }

        if ($prefixes) {
            foreach (@len_n) {
            push @{ $len_m{ $_->[  0 ] } }, $_;
            }
        } elsif ($suffixes) {
            foreach (@len_n) {
            push @{ $len_m{ $_->[ -1 ] } }, $_;
            }
        }

        foreach (sort keys %len_m) {
            if (@{ $len_m{ $_ } } > 1) {
            push @alt_n,
                             _presuf($level + 1, $param, @{ $len_m{ $_ } });
            } else {
            push @alt_n, join('', @{ $len_m{ $_ }->[0] });
            }
        }
        }
    }

    if (@len_1) { # Character classes.
        if ($param->{ anychar } and
        (exists $pre_d{ '.' } or exists $suf_d{ '.' }) and
            grep { $_->[0] eq '.' } @len_1) {
        push @alt_1, '.';
        } else {
        if (@len_1 == 1) {
            push @alt_1,
                         join('', @{$len_1[0]});
        } else {
            my %uniq;
            push @alt_1,
                         join('', '[', (sort
                    grep { ! $uniq{$_}++ }
                    map { join('', @$_) } @len_1), ']' );
        }
        }
    }

    my $alt = join('|', @alt_n, @alt_1);

    $alt = '(?:' . $alt . ')' unless @alt_n == 0;

    $alt .= '?' if $len_0;

    print "_presuf:$INDENT -> $alt\n" if $DEBUG;

    return $alt;
    }
}

sub presuf {
    my $param = ref $_[0] eq 'HASH' ? shift : { };

    return '' if @_ == 0;

    my @args = map { quotemeta() } @_;

    # Undo quotemeta for anychars.
    @args = map { s/\\\././g; $_ } @args if $param->{ anychar };

    s/\\(\s)/$1/g for @args;

    foreach (@args) {
    $_ = [ /(\\?.)/gs ];
    }

    return _presuf(0, $param, @args);
}
