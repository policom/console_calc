#pragma once
#ifdef _WIN32
    #define PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
    #define PLUGIN_EXPORT extern "C"
#endif

using MathFunction = double (*)(double);

struct PluginInfo {
    const char* name;
    MathFunction function;
};

using GetPluginInfo = const PluginInfo* (*)();
