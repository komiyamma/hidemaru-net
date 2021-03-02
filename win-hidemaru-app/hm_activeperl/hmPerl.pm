#--------------------------------------------------------------
# hmPerl 1.8.0.1用 カスタムファイル
#                 ActivePerl x86版 v5.10 ～ v5.26
#
# Copyright (c) 2016-2021 Akitsugu Komiyama
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
        sub GetEncoding {
            my ($self, $value) = @_;
            $::hm_File_HmEncode = $value;
            my $hm_encode = $::hm_File_HmEncode_Result + 0;
            my $ms_codepage = 0;
            my @ms_codepage_list = (
                0,      # Unknown
                932,    # encode = 1 ANSI/OEM Japanese; Japanese (Shift-JIS)
                1200,   # encode = 2 Unicode UTF-16, little-endian
                51932,  # encode = 3 EUC
                50221,  # encode = 4 JIS
                65000,  # encode = 5 UTF-7
                65001,  # encode = 6 UTF-8
                1201,   # encode = 7 Unicode UTF-16, big-endian
                1252,   # encode = 8 欧文 ANSI Latin 1; Western European (Windows)
                936,    # encode = 9 簡体字中国語 ANSI/OEM Simplified Chinese (PRC, Singapore); Chinese Simplified (GB2312)
                950,    # encode =10 繁体字中国語 ANSI/OEM Traditional Chinese (Taiwan; Hong Kong SAR, PRC); Chinese Traditional (Big5)
                949,    # encode =11 韓国語 ANSI/OEM Korean (Unified Hangul Code)
                1361,   # encode =12 韓国語 Korean (Johab)
                1250,   # encode =13 中央ヨーロッパ言語 ANSI Central European; Central European (Windows)
                1257,   # encode =14 バルト語 ANSI Baltic; Baltic (Windows)
                1253,   # encode =15 ギリシャ語 ANSI Greek; Greek (Windows)
                1251,   # encode =16 キリル言語 ANSI Cyrillic; Cyrillic (Windows)
                42,     # encode =17 シンボル
                1254,   # encode =18 トルコ語 ANSI Turkish; Turkish (Windows)
                1255,   # encode =19 ヘブライ語 ANSI Hebrew; Hebrew (Windows)
                1256,   # encode =20 アラビア語 ANSI Arabic; Arabic (Windows)
                874,    # encode =21 タイ語 ANSI/OEM Thai (same as 28605, ISO 8859-15); Thai (Windows)
                1258,   # encode =22 ベトナム語 ANSI/OEM Vietnamese; Vietnamese (Windows)
                10001,  # encode =23 x-mac-japanese Japanese (Mac)
                850,    # encode =24 OEM/DOS
                0,      # encode =25 その他
                12000,  # encode =26 Unicode (UTF-32) little-endian
                12001,  # encode =27 Unicode (UTF-32) big-endian
            );
            # 最小範囲以下
            if ($hm_encode <=0 ) {
                $ms_codepage = 0;
            }
            # 最大範囲以上
            my $ms_codepage_list_length = @ms_codepage_list;
            if ($hm_encode >= $ms_codepage_list_length) {
                $ms_codepage = 0;
            }

            $ms_codepage = $ms_codepage_list[$hm_encode];

            return {'HmEncode'=>$hm_encode, 'MsCodePage'=>$ms_codepage };
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
            my $result = $::hm_File_Load_Result;
            if ($result eq '-*-*-LoadFileUnicodeError-*-*-') {
                return undef;
            } else {
                return $result;
            }
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


# アウトプットパネル周り
#--------------------------------------------------------------
sub OutputPane {
    my ($class) = @_;
    return bless {}, $class;
}

        #--------------------------------------------------------------
        sub Output {
            my ($self, $value) = @_;
            $::hm_OutputPane_Output = $value;
        }

        sub Push {
            my ($self, $value) = @_;
            $::hm_OutputPane_Push = "1";
        }

        sub Pop {
            my ($self, $value) = @_;
            $::hm_OutputPane_Pop = "1";
        }

        sub Clear {
            my ($self, $value) = @_;
            $::hm_OutputPane_Clear = "1";
        }

        sub SendMessage {
            my ($self, $value) = @_;
            $::hm_OutputPane_SendMessage = $value;
        }

        #--------------------------------------------------------------

    #--------------------------------------------------------------


package main;