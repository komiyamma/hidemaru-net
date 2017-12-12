/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */

#pragma once

//-------------------------------------------------------------------------
// 「秀丸デバッグモニター」がcp932のみ対応なので「utf16」を受け取って「cp932」にして出力する処理。
// cp932に収まらない文字は「?」となってしまうのはやむを得ない。
void OutputDebugStream(wstring wstr);

