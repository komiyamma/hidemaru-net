namespace hmV8DynamicNS
{
    public interface IVirtualPath
    {
        string Identifier { get; }

        string ResolvePath();

        string ResolveDirectory();

        string ResolveExtension();

        string ResolveContent();
    }
}
