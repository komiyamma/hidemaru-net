/*
 * Copyright (c) 2015-2016 EdonGashi
 * https://github.com/EdonGashi/ShipScript
 */

using Microsoft.ClearScript;

namespace hmV8DynamicNS
{
    public interface IEventSource
    {
        [ScriptMember("connect")]
        IEventConnection Connect(object callback);
    }
}
