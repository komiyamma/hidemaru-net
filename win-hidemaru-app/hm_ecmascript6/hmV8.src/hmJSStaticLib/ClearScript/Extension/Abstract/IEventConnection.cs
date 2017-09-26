using Microsoft.ClearScript;

namespace hmV8DynamicNS
{
    public interface IEventConnection
    {
        [ScriptMember("disconnect")]
        void Disconnect();
    }
}
