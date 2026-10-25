#include "PluginAPI.h"
#include <cmath>
#include <stdexcept>

namespace {
double calculateSin(double degrees) {
    return std::sin(degrees * 3.14159265358979323846 / 180.0);
}
const PluginInfo info{"sin", &calculateSin};
}

PLUGIN_EXPORT const PluginInfo* get_plugin_info() {
    return &info;
}
