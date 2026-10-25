#include "PluginManager.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#define NOMINMAX
#include <windows.h>

namespace {
std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

bool isDll(const std::filesystem::path& path) {
    return lower(path.extension().string()) == ".dll";
}
}

PluginManager::~PluginManager() {
    unloadAll();
}

void PluginManager::unloadAll() noexcept {
    functions_.clear();
    for (auto it = plugins_.rbegin(); it != plugins_.rend(); ++it) {
        if (it->library) {
            FreeLibrary(static_cast<HMODULE>(it->library));
            it->library = nullptr;
        }
    }
    plugins_.clear();
}

void PluginManager::loadFromDirectory(const std::string& directory) {
    namespace fs = std::filesystem;
    const fs::path folder(directory);

    if (!fs::exists(folder) || !fs::is_directory(folder)) {
        std::cerr << "Предупреждение: папка plugins не найдена: "
                  << folder.string() << '\n';
        return;
    }

    std::vector<fs::path> paths;
    try {
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.is_regular_file() && isDll(entry.path())) {
                paths.push_back(entry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Не удалось прочитать папку плагинов: "
                  << e.what() << '\n';
        return;
    }

    std::sort(paths.begin(), paths.end());
    if (paths.empty()) {
        std::cerr << "Предупреждение: в папке plugins нет DLL-плагинов.\n";
        return;
    }

    for (const auto& path : paths) {
        HMODULE handle = LoadLibraryW(path.wstring().c_str());
        if (!handle) {
            std::cerr << "Пропущена DLL " << path.filename().string()
                      << ": не удалось загрузить библиотеку.\n";
            continue;
        }

        auto getInfo = reinterpret_cast<GetPluginInfo>(
            GetProcAddress(handle, "get_plugin_info"));

        if (!getInfo) {
            std::cerr << "Пропущена DLL " << path.filename().string()
                      << ": отсутствует get_plugin_info.\n";
            FreeLibrary(handle);
            continue;
        }

        try {
            const PluginInfo* info = getInfo();
            if (!info || !info->name || !info->function) {
                std::cerr << "Пропущена DLL " << path.filename().string()
                          << ": некорректная информация о функции.\n";
                FreeLibrary(handle);
                continue;
            }

            const std::string name = lower(info->name);
            if (name.empty() || functions_.count(name)) {
                std::cerr << "Пропущена DLL " << path.filename().string()
                          << ": пустое или повторяющееся имя функции.\n";
                FreeLibrary(handle);
                continue;
            }

            plugins_.push_back({handle, name, info->function});
            functions_.emplace(name, info->function);
            std::cout << "Загружен плагин: " << name << '\n';
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при чтении плагина "
                      << path.filename().string() << ": " << e.what() << '\n';
            FreeLibrary(handle);
        } catch (...) {
            std::cerr << "Неизвестная ошибка при загрузке плагина "
                      << path.filename().string() << ".\n";
            FreeLibrary(handle);
        }
    }
}

bool PluginManager::hasFunction(const std::string& name) const {
    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return functions_.find(key) != functions_.end();
}

double PluginManager::call(const std::string& name, double argument) const {
    std::string key = name;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    auto it = functions_.find(key);
    if (it == functions_.end()) {
        throw std::runtime_error("Неизвестная функция: " + name);
    }

    // Исключения C++ не должны пересекать границу DLL; все плагины
    // из этого проекта сообщают об ошибках через std::runtime_error.
    const double result = it->second(argument);
    if (!std::isfinite(result)) {
        throw std::runtime_error("Функция '" + name +
                                 "' вернула недопустимый результат.");
    }
    return result;
}

std::size_t PluginManager::loadedCount() const {
    return plugins_.size();
}
