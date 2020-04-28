#include "VNA.hpp"

#include "Si5351C.hpp"
#include "max2871.hpp"
#include "main.h"
#include "Sampling.hpp"
#include "delay.hpp"

#define LOG_LEVEL	LOG_LEVEL_WARN
#define LOG_MODULE	"VNA"
#include "Log.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1, hspi3;

static Si5351C Si5351 = Si5351C(&hi2c1, 26000000);
static MAX2871 Source = MAX2871(&hspi1, PLL1_CE_GPIO_Port, PLL1_CE_Pin,
		PLL1_LE_GPIO_Port, PLL1_LE_Pin, PLL1_RF_GPIO_Port, PLL1_RF_Pin,
		PLL1_LD_GPIO_Port, PLL1_LD_Pin);
static MAX2871 LO1 = MAX2871(&hspi3, PLL2_CE_GPIO_Port, PLL2_CE_Pin,
		PLL2_LE_GPIO_Port, PLL2_LE_Pin, PLL2_RF_GPIO_Port, PLL2_RF_Pin,
		PLL2_LD_GPIO_Port, PLL2_LD_Pin);

static constexpr uint32_t IF1 = 60000000;
static constexpr uint32_t IF2 = 250000;

static void ExcitatePort1() {
	LOG_DEBUG("Exciting port1");
	TX_SWITCH_GPIO_Port->BSRR = TX_SWITCH_Pin;
}
static void ExcitatePort2() {
	LOG_DEBUG("Exciting port2");
	TX_SWITCH_GPIO_Port->BSRR = TX_SWITCH_Pin << 16;
}

bool VNA::Init() {
	Si5351.Init();
	Source.Init(100000000, false, 1, false);
	LO1.Init(100000000, false, 1, false);

	// Use Si5351 to generate referene frequencies for other PLLs and ADC
	Si5351.SetPLL(Si5351C::PLL::A, 800000000, Si5351C::PLLSource::XTAL);
	// Both MAX2871 get a 100MHz reference
	Si5351.SetCLK(0, 100000000, Si5351C::PLL::A);
	Si5351.Enable(0);
	Si5351.SetCLK(1, 100000000, Si5351C::PLL::A);
	Si5351.Enable(1);
	// Clock for ADC timing
	Si5351.SetCLK(7, Sampling::ADCClock, Si5351C::PLL::A);
	Si5351.Enable(7);
	// 10 MHz reference clock
	Si5351.SetCLK(6, 10000000, Si5351C::PLL::A, Si5351C::DriveStrength::mA8);
	Si5351.Enable(6);

	// Generate second LO with Si5351
	Si5351.SetCLK(2, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(2);
	Si5351.SetCLK(3, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(3);
	Si5351.SetCLK(4, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(4);

	Si5351.Locked(Si5351C::PLL::A);

	if(!Source.BuildVCOMap()) {
		LOG_WARN("Source VCO map failed");
	}
	if(!LO1.BuildVCOMap()) {
		LOG_WARN("LO1 VCO map failed");
	}

	Source.SetFrequency(1000000000);
	Source.SetPowerOutB(MAX2871::Power::p5dbm);
	Source.SetPowerOutA(MAX2871::Power::p2dbm);
	Source.UpdateFrequency();
	LO1.SetFrequency(1000000000 + IF1);
	LO1.SetPowerOutA(MAX2871::Power::p2dbm);
	LO1.SetPowerOutB(MAX2871::Power::p2dbm);
	LO1.UpdateFrequency();


	Source.RFEnable(true);
	LO1.RFEnable(true);

//	while(1);
//
//	while (1) {
//		// Sweep test
//		constexpr uint64_t start = 100000000;
//		constexpr uint64_t stop = 3200000000;
//		constexpr uint16_t steps = 1001;
//		constexpr uint64_t step = (stop - start) / (steps - 1);
//		uint32_t starttime = HAL_GetTick();
//		uint32_t waittime = 0, calctime = 0, comtime = 0;
//		for (uint64_t f = start; f <= stop; f += step) {
//			uint32_t calcstart = HAL_GetTick();
//			Source.SetFrequency(f);
//			calctime += HAL_GetTick() - calcstart;
//			uint32_t comstart = HAL_GetTick();
//			Source.UpdateFrequency();
//			comtime += HAL_GetTick() - comstart;
//			uint32_t waitstart = HAL_GetTick();
//			while (!Source.Locked())
//				;
//			waittime += HAL_GetTick() - waitstart;
//		}
//		uint32_t duration = HAL_GetTick() - starttime;
//		LOG_INFO("Sweeped %u points in %lums, (calc/com/wait: %lu/%lu/%lu)",
//				steps, duration, calctime, comtime, waittime);
//	}

	Sampling::Init();

	LOG_INFO("Initialized");
	return true;
}

static VNA::Callback callback;
static VNA::Result result;
static uint8_t activePort;
static uint32_t samples;

static void SamplingCallback(Sampling::Result res) {
	// Calculate magnitude and phase relative to reference
	if (activePort == 1) {
		activePort = 2;
		ExcitatePort2();
//		Delay::ms(1);
		Sampling::Start(SamplingCallback, samples);
		result.S11 = res.Port1 / res.Ref;
		result.S21 = res.Port2 / res.Ref;
	} else {
		result.S12 = res.Port1 / res.Ref;
		result.S22 = res.Port2 / res.Ref;
		LOG_DEBUG("Result:");
		LOG_DEBUG("S11: %f/%f", real(result.S11), imag(result.S11));
		LOG_DEBUG("S21: %f/%f", real(result.S21), imag(result.S21));
		LOG_DEBUG("S12: %f/%f", real(result.S12), imag(result.S12));
		LOG_DEBUG("S22: %f/%f", real(result.S22), imag(result.S22));
		if (callback) {
			callback(result);
		}
	}
}

bool VNA::Measure(uint64_t f, Callback cb, uint32_t nsamples) {
	LOG_INFO("Starting measurement at f=%lu%06luHz", (uint32_t ) (f / 1000000),
			(uint32_t ) (f % 1000000));
	callback = cb;
	Source.SetFrequency(f);
	Source.UpdateFrequency();
	LO1.SetFrequency(f + IF1);
	LO1.UpdateFrequency();
	ExcitatePort1();
	activePort = 1;
	while(!LO1.Locked() || !Source.Locked());
//	Delay::ms(1);
	samples = nsamples;
	Sampling::Start(SamplingCallback, samples);
	return true;
}
