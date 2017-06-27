import java.awt.*;

public class Hm {
	static int count;
	// ライブラリをロード
	static {System.loadLibrary("hmJavaVM");}
	
	public native void DebugInfo(String message);

	public native String MacroVarGetObj(String name);

	public native boolean MacroVarSetNum(String name, long value);

	public native boolean MacroVarSetStr(String name, String value);

	public static void main(String[] args) {
		Hm hm = new Hm();

		int a = 1 / 0;
		Hm.count++;
		
		String vv = hm.MacroVarGetObj("filename2");
		hm.DebugInfo("あああ");
		hm.DebugInfo(Hm.count + "");
		hm.MacroVarSetNum("#test1" , 100 );
		hm.MacroVarSetStr("$test1" , "500aaあいうえお" );
		System.out.println(vv);
		WindowTest windowText=new WindowTest();
	}
}

//
// WindowTest クラスは, Windowの表示を行います。
//
class WindowTest{

   //コンストラクタ
  WindowTest(){

     //Windowを作る
    Frame frame;//Frame 型変数を宣言
    frame=new Frame("Window Test");//Frame型変数に初期値をセット
    frame.setSize(240, 240);//サイズを指定す
    frame.setVisible(true);//可視化する

    //WIndowに表示する。
    Label label;//表示用の部品（ここではラベル）を宣言
    label=new Label("Hellow Window");//表示用部品に初期値をセット
    frame.add(label);//表示用の部品をWindowに追加
  }
 }