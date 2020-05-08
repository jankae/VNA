#include <VNA.hpp>
#include "Si5351C.hpp"
#include "max2871.hpp"
#include "main.h"
#include "delay.hpp"
#include "FPGA.hpp"
#include <complex>
#include "Exti.hpp"

#define LOG_LEVEL	LOG_LEVEL_WARN
#define LOG_MODULE	"VNA"
#include "Log.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi3;

static Si5351C Si5351 = Si5351C(&hi2c1, 26000000);
static MAX2871 Source = MAX2871(&hspi3, FPGA_CS_GPIO_Port, FPGA_CS_Pin);
static MAX2871 LO1 = MAX2871(&hspi3, FPGA_CS_GPIO_Port, FPGA_CS_Pin);

static constexpr uint32_t IF1 = 60000000;
static constexpr uint32_t IF2 = 250000;

static VNA::Callback callback;
static Protocol::SweepSettings settings;
static uint16_t pointCnt;
static bool excitingPort1;
static Protocol::Datapoint data;

static void ReadComplete(FPGA::SamplingResult result) {
	auto port1 = std::complex<float>(result.P1I, result.P1Q);
	auto port2 = std::complex<float>(result.P2I, result.P2Q);
	auto ref = std::complex<float>(result.RefI, result.RefQ);
	port1 /= ref;
	port2 /= ref;
	if(excitingPort1) {
		data.pointNum = pointCnt;
		data.frequency = settings.f_start + (settings.f_stop - settings.f_start) * pointCnt / (settings.points - 1);
		data.real_S11 = port1.real();
		data.imag_S11 = port1.imag();
		data.real_S21 = port2.real();
		data.imag_S21 = port2.imag();
	} else {
		data.real_S12 = port1.real();
		data.imag_S12 = port1.imag();
		data.real_S22 = port2.real();
		data.imag_S22 = port2.imag();
		if (callback) {
			callback(data);
		}
		pointCnt++;
		if (pointCnt >= settings.points) {
			// reached end of sweep, start again
			pointCnt = 0;
			FPGA::StartSweep();
		}
	}
	excitingPort1 = !excitingPort1;
}

static void FPGA_Interrupt(void*) {
	FPGA::InitiateSampleRead(ReadComplete);
}

bool VNA::Init(Callback cb) {
	callback = cb;
	Si5351.Init();

	// Use Si5351 to generate reference frequencies for other PLLs and ADC
	Si5351.SetPLL(Si5351C::PLL::A, 800000000, Si5351C::PLLSource::XTAL);
	// Both MAX2871 get a 100MHz reference
	Si5351.SetCLK(2, 100000000, Si5351C::PLL::A);
	Si5351.Enable(2);
	Si5351.SetCLK(3, 100000000, Si5351C::PLL::A);
	Si5351.Enable(3);
	// 50MHz FPGA clock
	Si5351.SetCLK(7, 50000000, Si5351C::PLL::A);
	Si5351.Enable(7);
	// 10 MHz external reference clock
	Si5351.SetCLK(6, 10000000, Si5351C::PLL::A, Si5351C::DriveStrength::mA8);
	Si5351.Enable(6);

	// Generate second LO with Si5351
	Si5351.SetCLK(1, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(1);
	Si5351.SetCLK(4, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(4);
	Si5351.SetCLK(5, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(6);

	Si5351.Locked(Si5351C::PLL::A);

	// FPGA clock is now present, can initialize
	FPGA::Init();

	// Enable new data interrupt
	FPGA::WriteRegister(FPGA::Reg::InterruptMask, 0x0004);

	Exti::SetCallback(FPGA_INTR_GPIO_Port, FPGA_INTR_Pin, Exti::EdgeType::Rising, Exti::Pull::Down, FPGA_Interrupt);

	// Initialize PLLs and build VCO maps
	FPGA::SetMode(FPGA::Mode::SourcePLL);
	Source.Init(100000000, false, 1, false);
	Source.SetPowerOutA(MAX2871::Power::p5dbm);
	// output B is not used
	Source.SetPowerOutB(MAX2871::Power::n4dbm, false);
	if(!Source.BuildVCOMap()) {
		LOG_WARN("Source VCO map failed");
	}
	Source.SetFrequency(1000000000);
	Source.UpdateFrequency();
	FPGA::SetMode(FPGA::Mode::LOPLL);
	LO1.Init(100000000, false, 1, false);
	LO1.SetPowerOutA(MAX2871::Power::p2dbm);
	LO1.SetPowerOutB(MAX2871::Power::p2dbm);
	if(!LO1.BuildVCOMap()) {
		LOG_WARN("LO1 VCO map failed");
	}
	LO1.SetFrequency(1000000000 + IF1);
	LO1.UpdateFrequency();

	FPGA::SetMode(FPGA::Mode::FPGA);
	FPGA::WriteMAX2871Default(Source.GetRegisters());

	LOG_INFO("Initialized");
	return true;
}

bool ConfigureSweep(Protocol::SweepSettings s) {
	settings = s;
	// Abort possible active sweep first
	FPGA::AbortSweep();
	uint16_t points = settings.points <= FPGA::MaxPoints ? settings.points : FPGA::MaxPoints;
	// Configure sweep
	FPGA::WriteRegister(FPGA::Reg::SweepPoints, points);
	uint32_t samplesPerPoint = (1000000 / s.if_bandwidth) & 0x0001FFFF;
	FPGA::WriteRegister(FPGA::Reg::SamplesPerPoint, samplesPerPoint & 0xFFFF);
	// Enable mixers/amplifier/PLLs
	uint16_t ctrlReg = 0xFC00 | (samplesPerPoint >> 16);
	FPGA::WriteRegister(FPGA::Reg::SystemControl, ctrlReg);
	// Transfer PLL configuration to FPGA
	for (uint16_t i = 0; i < points; i++) {
		uint64_t freq = s.f_start + (s.f_stop - s.f_start) * i / (s.points - 1);
		// SetFrequency only manipulates the register content in RAM, no SPI communication is done.
		// No mode-switch of FPGA necessary here.
		Source.SetFrequency(freq);
		LO1.SetFrequency(freq + IF1);
		FPGA::WriteSweepConfig(i, Source.GetRegisters(), LO1.GetRegisters(), 60, freq);
	}
	pointCnt = 0;
	excitingPort1 = true;
	// Start the sweep
	FPGA::StartSweep();
	return true;
}

