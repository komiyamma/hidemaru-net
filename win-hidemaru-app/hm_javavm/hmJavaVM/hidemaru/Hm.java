/*
 * Copyright (c) 2017 Akitsugu Komiyama
 * under the Apache License Version 2.0
 */
 
 package hidemaru;

import java.io.File;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.Collections;
import java.util.Map;
import java.util.HashMap;

public class Hm {

	// ライブラリをロード
	static { 
		System.loadLibrary("hmJavaVM");
	}

	// staticコンストラクタを強制実行させるために必要。
	protected static void _Init(String[] _dummy) {
	}

	
	protected static native double GetVersion();

	protected static native void DebugInfo(String expression);
	protected static native long GetWindowHandle();

	protected static native int EvalMacro(String message);

	protected static native String GetMacroVarObj(String name);
	protected static native int SetMacroVarNum(String name, long value);
	protected static native int SetMacroVarStr(String name, String value);

	protected static native String GetFileFullPath();
	protected static native String GetTotalText();
	protected static native int SetTotalText(String text);

	protected static native String GetSelectedText();
	protected static native int SetSelectedText(String text);

	protected static native String GetLineText();
	protected static native int SetLineText(String text);

	protected static native String GetCursorPos();
	protected static native String GetCursorPosFromMousePos();

	protected static native boolean IsMacroExecuting();
	protected static native String ExecMacroFromFile(String filename);
	protected static native String ExecMacroFromString(String expression);

	public static double getVersion() {
		return GetVersion();
	}

	public static long getWindowHandle() {
		return GetWindowHandle();
	}

	public static void debugInfo(Object text, Object... args) {
		String begin = text.toString();

		for( Object arg : args ) {
			begin += arg.toString();
		}
		DebugInfo(begin);
	}

	public static class Edit {
		public static File getFile() {
			String path = GetFileFullPath();
			if (path.length() > 0) {
			    return new File(path);
			} else {
			    return null;
			}
		}

		public static long getWindowHandle() {
			return GetWindowHandle();
		}

		public static String getTotalText() {
			return GetTotalText();
		}
		public static boolean setTotalText(Object value) {
			int ret = SetTotalText(value.toString());
			if (ret == 0) {
				throw new java.lang.RuntimeException("Can't Edit Hidemaru TotalText");
			}
			return true;
		}
		public static String getSelectedText() {
			return GetSelectedText();
		}
		public static boolean setSelectedText(Object value) {
			int ret = SetSelectedText(value.toString());
			if (ret == 0) {
				throw new java.lang.RuntimeException("Can't Edit Hidemaru SelectedText");
			}
			return true;
		}
		public static String getLineText() {
			return GetLineText();
		}
		public static boolean setLineText(Object value) {
			int ret = SetLineText(value.toString());
			if (ret == 0) {
				throw new java.lang.RuntimeException("Can't Edit Hidemaru LineText");
			}
			return true;
		}

		public static class CursorPos {
			private int m_lineno;
			private int m_column;

			public CursorPos(int lineno, int column) {
				this.m_lineno = lineno;
				this.m_column = column;
			}

			public int getLineNo() {
				return m_lineno;
			}
			public int getColumn() {
				return m_column;
			}
		}

		public static CursorPos getCursorPos() {
			String strPos = GetCursorPos();
			String[] lineno_and_column = strPos.split(",", 2);
			int lineno = Integer.parseInt(lineno_and_column[0]);
			int column = Integer.parseInt(lineno_and_column[1]);
			CursorPos pos = new CursorPos(lineno, column);
			return pos;
		}

		public static class MousePos {
			private int m_x;
			private int m_y;
			private int m_lineno;
			private int m_column;

			public MousePos(int x, int y, int lineno, int column) {
				this.m_x = x;
				this.m_y = y;
				this.m_lineno = lineno;
				this.m_column = column;
			}

			public int getX() {
				return m_x;
			}
			public int getY() {
				return m_y;
			}
			public int getLineNo() {
				return m_lineno;
			}
			public int getColumn() {
				return m_column;
			}
		}
		public static MousePos getMousePos() {
			String strPos = GetCursorPosFromMousePos();
			String[] x_and_y_and_lineno_and_column = strPos.split(",", 4);
			int x = Integer.parseInt(x_and_y_and_lineno_and_column[0]);
			int y = Integer.parseInt(x_and_y_and_lineno_and_column[1]);
			int lineno = Integer.parseInt(x_and_y_and_lineno_and_column[2]);
			int column = Integer.parseInt(x_and_y_and_lineno_and_column[3]);
			MousePos pos = new MousePos(x, y, lineno, column);
			return pos;
		}

	}

	public static class Macro {

	    public static boolean isExecuting() {
			return IsMacroExecuting();
		}
		
		public static Map<String, Object> doExec(File file) {
			String filename = file.getAbsolutePath();
		    String str_result = ExecMacroFromFile(filename);
			String[] splited_result = str_result.split(",", 3);
			int ret = Integer.parseInt(splited_result[0]);
			String exception = splited_result[1];
			String message = splited_result[2];

			if (!file.exists()) {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", 0 );
				result.put( "message", message);
				result.put( "exception", new java.io.FileNotFoundException(filename) );
				return Collections.unmodifiableMap(result);
			}

			if (ret == -1) {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", -1 );
				result.put( "message", message);
				result.put( "exception", new java.lang.RuntimeException(exception) );
				return Collections.unmodifiableMap(result);
			}
			if (ret == 0) {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", 0 );
				result.put( "message", message );
				result.put( "exception", new java.lang.RuntimeException(exception) );
				return Collections.unmodifiableMap(result);
			}

			Map<String, Object> result = new HashMap<>();
			result.put( "result", ret );
			result.put( "message", message );
			result.put( "exception", null );

			return Collections.unmodifiableMap(result);
		}

		public static Map<String, Object> doExec(Object expression) {
		    String str_result = ExecMacroFromString(expression.toString());
			String[] splited_result = str_result.split(",", 3);
			int ret = Integer.parseInt(splited_result[0]);
			String exception = splited_result[1];
			String message = splited_result[2];

			if (ret == -1) {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", -1 );
				result.put( "message", message);
				result.put( "exception", new java.lang.RuntimeException(exception) );
				return Collections.unmodifiableMap(result);
			}
			if (ret == 0) {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", 0 );
				result.put( "message", message);
				result.put( "exception", new java.lang.RuntimeException(exception) );
				return Collections.unmodifiableMap(result);
			}

			Map<String, Object> result = new HashMap<>();
			result.put( "result", ret );
			result.put( "message", message );
			result.put( "exception", null );

			return Collections.unmodifiableMap(result);
		}

		public static Map<String, Object> doEval(Object expression) {
			int ret = EvalMacro(expression.toString());
			// エラー
			if (ret==0) {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", 0 );
				result.put( "message", "" );
				result.put( "exception", new java.lang.RuntimeException("HidemaruMacroEvalException") );
				return Collections.unmodifiableMap(result);
			} else {
				Map<String, Object> result = new HashMap<>();
				result.put( "result", ret );
				result.put( "message", "" );
				result.put( "exception", null );

				return Collections.unmodifiableMap(result);
			}
		}

		public static Object getVar(String symbol) {
			String result = GetMacroVarObj(symbol);
			if (result.contains("HmJavaVMType_Int32<<>>")) {
				result = result.replace("HmJavaVMType_Int32<<>>", "");
				int num = Integer.parseInt(result);
				return num;
			} else if (result.contains("HmJavaVMType_Int64<<>>")) {
				result = result.replace("HmJavaVMType_Int64<<>>", "");
				long num = Long.parseLong(result);
				return num;
			} else if (result.contains("HmJavaVMType_String<<>>")) {
				result = result.replace("HmJavaVMType_String<<>>", "");
				return result;
			}

			throw new java.lang.IllegalArgumentException("Can't find Hidemaru Macro Simbol " + symbol);
		}

		public static boolean setVar(String symbol, Object value) {
			int ret = 0;
			if (symbol.startsWith("#")) {
				String strValue = value.toString();
				long num = new Long(strValue).longValue();
				ret = SetMacroVarNum(symbol, num);
			}
			else if (symbol.startsWith("$")) {
				String str = value.toString();
				ret = SetMacroVarStr(symbol, str);
			}

			if (ret != 0) {
				return true;
			} else {
				throw new java.lang.IllegalArgumentException("Can't assign Hidemaru Macro Simbol " + symbol);
			}
		}
	}

	/**
	 * マクロが存在するパスの追加。
	 * そのパスのlibサブディレクトリの追加。
	 */
	protected static void _AddClassPath(String[] _dummy){

		String currentmacrodirectory = (String)Hm.Macro.getVar("currentmacrodirectory");
		_AddClassPath(currentmacrodirectory);

        // ディレクトリ配下を探索
        File[] filelist = new File(currentmacrodirectory).listFiles();
        for (File file : filelist) {
			if (file.isFile()) {
				String suffix = _GetFiNameSuffix(file.getName());
				// 見つかったファイルがJARの場合は追加
				if (suffix.equalsIgnoreCase("jar")) {
					_AddClassPath(currentmacrodirectory + "\\" + file.getName());
				}
			}
        }

	}

	/**
	 * ClassPathの追加
	 * @param path 追加するPATH
	 */
    private static void _AddClassPath(String path) {

		URLClassLoader loader = (URLClassLoader)ClassLoader.getSystemClassLoader(); // Java9ではエラー

		try {
			URL u = new File(path).toURI().toURL();
 			Method m = URLClassLoader.class.getDeclaredMethod("addURL", new Class[]{ URL.class }); // Java9ではエラー
			m.setAccessible(true);
			m.invoke(loader, new Object[]{u});
			Hm.debugInfo("ClassPathに「" + path + "」を追加しました");
		} catch (Exception e){
			// 失敗は予測されること。いちいち止めない。
			Hm.debugInfo( new RuntimeException("ClassPathの追加に失敗しました。(" + path + ")" ) );
		}
	}

	/**
	 * ファイル名から拡張子を取り除いた名前を返します。
	 * @param fileName ファイル名
	 * @return ファイル名
	 */
	private static String _GetFiNamePrefix(String fileName) {
		if (fileName == null) {
			return null;
		}
		int point = fileName.lastIndexOf(".");
		if (point != -1) {
			return fileName.substring(0, point);
		} 
		return fileName;
	}

    /**
    * ファイル名から拡張子を返却
    * @param fileName ファイル名
    * @return ファイルの拡張子
    */
    private static String _GetFiNameSuffix(String fileName) {
        if (fileName == null) {
            return null;
        }
        int point = fileName.lastIndexOf(".");
        if (point != -1) {
            return fileName.substring(point + 1);
        }
        return fileName;
    }
}

