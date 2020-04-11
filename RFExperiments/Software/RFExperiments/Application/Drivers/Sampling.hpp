#pragma once

#include <cstdint>
#include <complex>

namespace Sampling {

static constexpr uint16_t MaxSamples = 32768;

using Result = struct _result {
	std::complex<float> Ref;
	std::complex<float> Port1;
	std::complex<float> Port2;
};

using Callback = void(*)(Result res);

bool Start(Callback cb, uint16_t samples);

}
