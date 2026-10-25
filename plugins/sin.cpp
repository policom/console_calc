#include "PluginAPI.h"
#include <cmath>
#include <stdexcept>

namespace {
double calculateSin(double degrees) {
    // По примеру из ТЗ sin(90) должен вычисляться как 1:
    // аргумент задаётся в градусах.
    return std::sin(degrees * 3.14159265358979323846 / 180.0);
}
const PluginInfo info{"sin", &calculateSin};
}

PLUGIN_EXPORT const PluginInfo* get_plugin_info() {
    return &info;
}
