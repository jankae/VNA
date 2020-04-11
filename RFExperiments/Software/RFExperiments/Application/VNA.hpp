#pragma once

#include <cstdint>

namespace VNA {

using Result = struct _result {
	float S11Mag, S11Phase;
	float S21Mag, S21Phase;
	float S12Mag, S12Phase;
	float S22Mag, S22Phase;
};

using Callback = void(*)(Result);

bool Init();
bool Measure(uint64_t f, Callback cb, uint16_t nsamples);

}
