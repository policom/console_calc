#include "PluginAPI.h"
#include <cmath>
#include <stdexcept>

namespace {
double calculateSqrt(double value) {
    if (value < 0.0) {
        throw std::runtime_error("Нельзя извлечь квадратный корень из отрицательного числа.");
    }
    return std::sqrt(value);
}
const PluginInfo info{"sqrt", &calculateSqrt};
}

PLUGIN_EXPORT const PluginInfo* get_plugin_info() {
    return &info;
}
