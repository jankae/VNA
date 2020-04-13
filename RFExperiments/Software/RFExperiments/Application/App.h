#pragma once

#ifdef __cplusplus
#include "Protocol.hpp"

namespace App {

void NewSettings(Protocol::SweepSettings s);

}

extern "C" {
#endif

void App_Start();

#ifdef __cplusplus
}
#endif
