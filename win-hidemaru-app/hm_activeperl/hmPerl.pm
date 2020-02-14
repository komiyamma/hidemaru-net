#--------------------------------------------------------------
# hmPerl 1.6.0.1用 カスタムファイル
#                 ActivePerl x86版 v5.10 ～ v5.26
#
# Copyright (c) 2016-2020 Akitsugu Komiyama
#                 under the Apache License Version 2.0
#
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

sub version {
    return $::hm_version + 0;
}

# 秀丸のオプションに従ったエンコードを得る
#--------------------------------------------------------------
sub File {
    my ($class) = @_;
    return bless {}, $class;
}

        #--------------------------------------------------------------
        sub GetHmEncode {
            my ($self, $value) = @_;
            $::hm_File_HmEncode = $value;
            return $::hm_File_HmEncode_Result;
        }

        #--------------------------------------------------------------
        sub ReadAllText {
            my ($self, $filename, $value) = @_;
            if ($value eq "") {
                $::hm_File_HmEncode = $filename;
                $value = $::hm_File_HmEncode_Result;
            }
            $::hm_File_Load_Encode = $value;
            $::hm_File_Load_FileName = $filename;
            return $::hm_File_Load_Result;
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

        sub MousePos {
            my ($self) = @_;
            my ($x, $y, $lineno, $column) = $::hm_Edit_CursorPosFromMousePos =~ /^(.+?),(.+?),(.+?),(.+)$/;
            return {'x'=>$x, 'y'=>$y, 'lineno'=>$lineno, 'column'=>$column };
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
            my $last_error_msg = $::hm_Macro_Eval_Result;
            if ($last_error_msg) {
                return {'Result'=>0, 'Message'=>"", 'Error'=>"HidemaruMacroEvalException" };
            } else {
                return {'Result'=>1, 'Message'=>"", 'Error'=>"" };
            }
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