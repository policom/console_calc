#include "PluginAPI.h"
#include <cmath>

namespace {
double calculateCos(double degrees) {
    // Как и sin, функция принимает угол в градусах.
    constexpr double pi = 3.14159265358979323846;
    return std::cos(degrees * pi / 180.0);
}

const PluginInfo info{"cos", &calculateCos};
}

PLUGIN_EXPORT const PluginInfo* get_plugin_info() {
    return &info;
}
