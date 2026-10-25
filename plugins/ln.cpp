#include "PluginAPI.h"
#include <cmath>
#include <stdexcept>

namespace {
double calculateLn(double value) {
    if (value <= 0.0) {
        throw std::runtime_error("ln определён только для положительных чисел.");
    }
    return std::log(value);
}
const PluginInfo info{"ln", &calculateLn};
}

PLUGIN_EXPORT const PluginInfo* get_plugin_info() {
    return &info;
}

