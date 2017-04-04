#pragma once

// szNameがフルパスに含まれるプロセスＩＤを取得。最初に見つかったやつ。
// だが確実性にやや欠けるので、可能な限りクラス名(TARGET_CLASS_WND)で対処した方がよい。
int getTargetProcessID ( char *szNamePattern );

// 指定のファイル名のプロセス名が何個実行されいるかを得る
int getTargetProcessCount(char* process);