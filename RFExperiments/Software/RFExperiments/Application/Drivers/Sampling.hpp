#pragma once

#include <cstdint>
#include <complex>

namespace Sampling {

static constexpr double ADCClock = (800000000.0/60.0);//4000000.0f;
static constexpr double ADCDivider = 7.0;

using Result = struct _result {
	std::complex<float> Ref;
	std::complex<float> Port1;
	std::complex<float> Port2;
};

using Callback = void(*)(Result res);

bool Init();
bool Start(Callback cb, uint32_t samples);

}
