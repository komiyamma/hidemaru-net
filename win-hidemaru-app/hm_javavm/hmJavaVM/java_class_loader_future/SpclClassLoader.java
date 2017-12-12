/**
 * Java9では、URLClassLoaderはAppClassLoaderから継承していないので、addURLメソッドが無い
 * よって次のように独自に継承していてやる必要がある
 * これだけでは動作せず、もろもろ諸処は必要。
 * https://stackoverflow.com/questions/42052856/java-9-classpath-and-library-path-extension
 * 場所不定のプラグイン的な遅延ロードをしている人は結構多いハズなので、
 * java9リリース後、半年も経過すれば、安定したソースが確立されると思われる。
 */

public class MiscTools
{
    private static class SpclClassLoader extends URLClassLoader
    {
        static
        {
            ClassLoader.registerAsParallelCapable();
        }

        private final Set<Path> userLibPaths = new CopyOnWriteArraySet<>();

        private SpclClassLoader()
        {
            super(new URL[0]);
        }

        @Override
        protected void addURL(URL url)
        {
            super.addURL(url);
        }

        protected void addLibPath(String newpath)
        {
            userLibPaths.add(Paths.get(newpath).toAbsolutePath());
        }

        @Override
        protected String findLibrary(String libname)
        {
            String nativeName = System.mapLibraryName(libname);
            return userLibPaths.stream().map(tpath -> tpath.resolve(nativeName)).filter(Files::exists).map(Path::toString).findFirst().orElse(super.findLibrary(libname));            }
    }
    private final static SpclClassLoader ucl = new SpclClassLoader();

    /**
     * Adds a jar file or directory to the classpath. From Utils4J.
     *
     * @param newpaths JAR filename(s) or directory(s) to add
     * @return URLClassLoader after newpaths added if newpaths != null
     */
    public static ClassLoader addToClasspath(String... newpaths)
    {
        if (newpaths != null)
            try
            {
                for (String newpath : newpaths)
                    if (newpath != null && !newpath.trim().isEmpty())
                        ucl.addURL(Paths.get(newpath.trim()).toUri().toURL());
            }
            catch (IllegalArgumentException | MalformedURLException e)
            {
                RuntimeException re = new RuntimeException(e);
                re.setStackTrace(e.getStackTrace());
                throw re;
            }
        return ucl;
    }

    /**
     * Adds to library path in ClassLoader returned by addToClassPath
     *
     * @param newpaths Path(s) to directory(s) holding OS library files
     */
    public static void addToLibraryPath(String... newpaths)
    {
        for (String newpath : Objects.requireNonNull(newpaths))
            ucl.addLibPath(newpath);
    }
}