using Microsoft.ClearScript;

namespace hmV8DynamicNS
{
    public interface IEventSource
    {
        [ScriptMember("connect")]
        IEventConnection Connect(object callback);
    }
}
