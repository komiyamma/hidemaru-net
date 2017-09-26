namespace hmV8DynamicNS
{
    public interface IModulePathResolver
    {
        IVirtualPath Resolve(string path, Module parent);
    }
}
