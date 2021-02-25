# これは何？  
秀丸は過去の増改築の都合上、dllfunc などで、dllの関数への引数として wcharではなく char で呼ぶ際、
そのdllに「STARTUNIMACRO」関数が dllexport されているならば、引数が文字列なら特殊なエンコードを行う。

これは、そのエンコードの際、WChar(code=0)からWChar(code=0xffff) までどのようにエンコードされるかの65535個のリストファイルを出力するためのプログラム。

