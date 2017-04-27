#--------------------------------------------------------------
# hmPerl 1.2.2.1用 カスタムファイル
#                 ActivePerl x86版 v5.10.0.1004 ～ v5.24.1.2402
#--------------------------------------------------------------


#--------------------------------------------------------------
# 秀丸の各種変数の読み取りや書き込みを行う
#--------------------------------------------------------------

package hm;

#--------------------------------------------------------------

sub debuginfo {
    my ($self, $value) = @_;
	$::hm_debuginfo = $value;
}

# 編集中の秀丸のテキストを得たり、テキストを変更したりする
#--------------------------------------------------------------
sub Edit {
	my ($class) = @_;
	return bless {}, $class;
}

	    #--------------------------------------------------------------
		sub TotalText {
		    my ($self, $value) = @_;
			if ($#_ == 0) {
				return $::hm_Edit_TotalText;
			} else {
				$::hm_Edit_TotalText = $value;
			}
		}

		sub SelectedText {
		    my ($self, $value) = @_;
			if ($#_ == 0) {
				return $::hm_Edit_SelectedText;
			} else {
				$::hm_Edit_SelectedText = $value;
			}
		}

		sub LineText {
		    my ($self, $value) = @_;
			if ($#_ == 0) {
				return $::hm_Edit_LineText;
			} else {
				$::hm_Edit_LineText = $value;
			}
		}

		sub CursorPos {
		    my ($self) = @_;
			my ($lineno, $column) = $::hm_Edit_CursorPos =~ /^(.+?),(.+)$/;
			return {'lineno'=>$lineno, 'column'=>$column };
		}
	    #--------------------------------------------------------------

    #--------------------------------------------------------------



# 秀丸マクロをPerl内から実行したり、秀丸マクロ用の変数⇔Perl変数の相互やりとりを行う
#--------------------------------------------------------------
sub Macro {
	my ($class) = @_;
	return bless {}, $class;
}

	    #--------------------------------------------------------------
		sub Eval {
		    my ($self, $expression) = @_;
			$::hm_Macro_Eval = $expression;
		}

		sub Var {
		    my ($self, $simbol, $value) = @_;
			if ($#_ == 1) {
				$::hm_Macro_Var_Simbol = $simbol;
				return $::hm_Macro_Var_Simbol;
			} else {
				$::hm_Macro_Var_Simbol = $simbol;
				$::hm_Macro_Var_Value = $value;
			}
		}
	    #--------------------------------------------------------------

    #--------------------------------------------------------------



package main;