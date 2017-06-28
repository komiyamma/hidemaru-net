import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;

/**
 * クラスパスの動的追加
 *
 * @author $Author: U.Yobane $
 * @version $Version: v 043b2b764886 2011/06/13 01:23:25+0900(JST) $
 */
public final class ApplicationClassLoader {

    /** クラス情報格納フィールド */
    private static final Class<?>[] PARAMETERS = new Class<?>[] { URL.class };

    /**
    * コンストラクタ
    */
    private ApplicationClassLoader() {

    }

    /**
    * メイン
    *
    * @param args 起動引数文字列配列
    */
    public static void main(String[] args) {

        try {
            // クラスパスの追加
            addClassPath( "C:\\abcc" );
            addClassPath( "C:\\abcc\\Hm.zip" );

            // メインプログラムを実行
            // Application.execute(args);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
    * クラスローダーにクラスパスを動的追加
    *
    * @param classPath 追加するクラスパス
    * @throws IOException クラスパスの動的追加に失敗
    */
    public static void addClassPath(String classPath) throws IOException {
        addClassPath(new File(classPath));
    }

    /**
    * クラスパスを再帰的にたどり、追加可能パスはすべて追加
    *
    * @param classPath 追加するファイル
    * @throws IOException クラスパスの動的追加に失敗
    */
    private static void addClassPath(File classPath) throws IOException {
        if (classPath.isDirectory()) {

            // ディレクトリを追加
            addClassPath(classPath.toURI().toString());

            // ディレクトリ配下を探索
            File[] child = classPath.listFiles();
            for (int i = 0; i < child.length; i++) {
                addClassPath(child[i]);
            }
        } else {
            String suffix = getSuffix(classPath.getName());

            // 見つかったファイルがJARもしくはZipの場合は追加
            if (suffix.equalsIgnoreCase("zip") || suffix.equalsIgnoreCase("jar")) {
                addClassPath(classPath.toURI().toString());
            }
        }
    }

    /**
    * システムクラスローダーにクラスパスを強制的に追加
    *
    * @param classPathUrl クラスパス
    * @throws IOException クラスパスの動的追加に失敗
    */
    private static void addClassPath(URL classPathUrl) throws IOException {

        URLClassLoader sysloader = (URLClassLoader) ClassLoader.getSystemClassLoader();

        Class<?> sysclass = URLClassLoader.class;

        try {
            Method method = sysclass.getDeclaredMethod("addURL", PARAMETERS);
            method.setAccessible(true);
            method.invoke(sysloader, new Object[] { classPathUrl });
        } catch (NoSuchMethodException e) {
            throw new IOException("could not add " + classPathUrl
                + " to classpath");
        } catch (InvocationTargetException e) {
            throw new IOException("could not add " + classPathUrl
                + " to classpath");
        } catch (IllegalAccessException e) {
            throw new IOException("could not add " + classPathUrl
                + " to classpath");
        }
    }

    /**
    * ファイル名から拡張子を返却
    *
    * @param fileName ファイル名
    * @return ファイルの拡張子
    */
    private static String getSuffix(String fileName) {
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