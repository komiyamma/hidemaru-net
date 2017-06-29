import java.awt.*;

public class Test {

	

	// staticコンストラクタを強制実行させるために必要。
	public static void main(String[] args) {
		Hm.DebugInfo("OK");
		Hm.DebugInfo(Hm.Macro.getVar("filename2"));
	}
}
