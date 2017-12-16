/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using Microsoft.ClearScript;

namespace hmV8DynamicNS
{
    public interface IEventConnection
    {
        [ScriptMember("disconnect")]
        void Disconnect();
    }
}
