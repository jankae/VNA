#pragma once

#include <cstdint>
#include <complex>

namespace VNA {

using Result = struct _result {
	std::complex<float> S11;
	std::complex<float> S21;
	std::complex<float> S12;
	std::complex<float> S22;
};

using Callback = void(*)(Result);

bool Init();
bool Measure(uint64_t f, Callback cb, uint32_t nsamples);

}
