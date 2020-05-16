#pragma once

#include <cstdint>
#include "Protocol.hpp"

namespace VNA {

using Callback = void(*)(Protocol::Datapoint);

bool Init(Callback cb);
bool ConfigureSweep(Protocol::SweepSettings s);
bool GetTemps(uint8_t *source, uint8_t *lo);

}

