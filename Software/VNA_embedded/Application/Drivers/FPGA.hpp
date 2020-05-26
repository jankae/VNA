#pragma once

#include <cstdint>

namespace FPGA {

static constexpr uint16_t MaxPoints = 4501;

enum class Reg {
	InterruptMask = 0x00,
	SweepPoints = 0x01,
	SamplesPerPoint = 0x02,
	SystemControl = 0x03,
	SettlingTime = 0x04,
	MAX2871Def0LSB = 0x08,
	MAX2871Def0MSB = 0x09,
	MAX2871Def1LSB = 0x0A,
	MAX2871Def1MSB = 0x0B,
	MAX2871Def3LSB = 0x0C,
	MAX2871Def3MSB = 0x0D,
	MAX2871Def4LSB = 0x0E,
	MAX2871Def4MSB = 0x0F,
};

using SamplingResult = struct _samplingresult {
	int64_t P1I, P1Q;
	int64_t P2I, P2Q;
	int64_t RefI, RefQ;
};

using ADCLimits = struct _adclimits {
	int16_t P1min, P1max;
	int16_t P2min, P2max;
	int16_t Rmin, Rmax;
};

using HaltedCallback = void(*)(void);
bool Init(HaltedCallback cb = nullptr);
void WriteRegister(Reg reg, uint16_t value);
void WriteMAX2871Default(uint32_t *DefaultRegs);
void WriteSweepConfig(uint16_t pointnum, uint32_t *SourceRegs, uint32_t *LORegs,
		uint8_t attenuation, uint64_t frequency, bool halt = false);
using ReadCallback = void(*)(SamplingResult result);
bool InitiateSampleRead(ReadCallback cb);
ADCLimits GetADCLimits();
void ResetADCLimits();
void ResumeHaltedSweep();
uint16_t GetStatus();

void StartSweep();
void AbortSweep();

enum class Mode {
	FPGA,
	SourcePLL,
	LOPLL,
};
void SetMode(Mode mode);


}
