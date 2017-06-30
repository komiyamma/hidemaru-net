import java.lang.*;
import java.lang.reflect.Method;
import java.lang.Class;
import java.lang.ClassNotFoundException;

public class MiscTest {
	// ライブラリをロード
	static { 
		Thread.currentThread().setContextClassLoader(MiscTools.addToClasspath());
	}

	static void main(String args[]) {
	}
}