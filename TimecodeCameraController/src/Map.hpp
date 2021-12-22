#pragma once
#include <algorithm>

static float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return std::max(std::min((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min, out_max), out_min);
}