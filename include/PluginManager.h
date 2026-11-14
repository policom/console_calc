#pragma once
#include "PluginAPI.h"
#include <string>
#include <unordered_map>
#include <vector>

class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager();

    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    void loadFromDirectory(const std::string& directory);
    bool hasFunction(const std::string& name) const;
    double call(const std::string& name, double argument) const;
    std::size_t loadedCount() const;

private:
    struct LoadedPlugin {
        void* library = nullptr;
        std::string name;
        MathFunction function = nullptr;
    };

    std::vector<LoadedPlugin> plugins_;
    std::unordered_map<std::string, MathFunction> functions_;

    void unloadAll() noexcept;
};
