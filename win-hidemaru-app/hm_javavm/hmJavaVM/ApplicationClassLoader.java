import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.URL;
import java.net.URLClassLoader;

/**
 * �N���X�p�X�̓��I�ǉ�
 *
 * @author $Author: U.Yobane $
 * @version $Version: v 043b2b764886 2011/06/13 01:23:25+0900(JST) $
 */
public final class ApplicationClassLoader {

    /** �N���X���i�[�t�B�[���h */
    private static final Class<?>[] PARAMETERS = new Class<?>[] { URL.class };

    /**
    * �R���X�g���N�^
    */
    private ApplicationClassLoader() {

    }

    /**
    * ���C��
    *
    * @param args �N������������z��
    */
    public static void main(String[] args) {

        try {
            // �N���X�p�X�̒ǉ�
            addClassPath( "C:\\abcc" );
            addClassPath( "C:\\abcc\\Hm.zip" );

            // ���C���v���O���������s
            // Application.execute(args);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
    * �N���X���[�_�[�ɃN���X�p�X�𓮓I�ǉ�
    *
    * @param classPath �ǉ�����N���X�p�X
    * @throws IOException �N���X�p�X�̓��I�ǉ��Ɏ��s
    */
    public static void addClassPath(String classPath) throws IOException {
        addClassPath(new File(classPath));
    }

    /**
    * �N���X�p�X���ċA�I�ɂ��ǂ�A�ǉ��\�p�X�͂��ׂĒǉ�
    *
    * @param classPath �ǉ�����t�@�C��
    * @throws IOException �N���X�p�X�̓��I�ǉ��Ɏ��s
    */
    private static void addClassPath(File classPath) throws IOException {
        if (classPath.isDirectory()) {

            // �f�B���N�g����ǉ�
            addClassPath(classPath.toURI().toString());

            // �f�B���N�g���z����T��
            File[] child = classPath.listFiles();
            for (int i = 0; i < child.length; i++) {
                addClassPath(child[i]);
            }
        } else {
            String suffix = getSuffix(classPath.getName());

            // ���������t�@�C����JAR��������Zip�̏ꍇ�͒ǉ�
            if (suffix.equalsIgnoreCase("zip") || suffix.equalsIgnoreCase("jar")) {
                addClassPath(classPath.toURI().toString());
            }
        }
    }

    /**
    * �V�X�e���N���X���[�_�[�ɃN���X�p�X�������I�ɒǉ�
    *
    * @param classPathUrl �N���X�p�X
    * @throws IOException �N���X�p�X�̓��I�ǉ��Ɏ��s
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
    * �t�@�C��������g���q��ԋp
    *
    * @param fileName �t�@�C����
    * @return �t�@�C���̊g���q
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