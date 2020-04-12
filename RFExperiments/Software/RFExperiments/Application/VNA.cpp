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
extern SPI_HandleTypeDef hspi1;

static Si5351C Si5351 = Si5351C(&hi2c1, 26000000);
static MAX2871 Source = MAX2871(&hspi1, PLL1_CE_GPIO_Port, PLL1_CE_Pin,
		PLL1_LE_GPIO_Port, PLL1_LE_Pin, PLL1_RF_GPIO_Port, PLL1_RF_Pin,
		PLL1_LD_GPIO_Port, PLL1_LD_Pin);
static MAX2871 LO1 = MAX2871(&hspi1, PLL1_CE_GPIO_Port, PLL1_CE_Pin,
		PLL1_LE_GPIO_Port, PLL1_LE_Pin, PLL1_RF_GPIO_Port, PLL1_RF_Pin,
		PLL1_LD_GPIO_Port, PLL1_LD_Pin);

static constexpr uint32_t IF1 = 50000000;
static constexpr uint32_t IF2 = 250000;

static void ExcitatePort1() {
	LOG_DEBUG("Exciting port1");
	// TODO
}
static void ExcitatePort2() {
	LOG_DEBUG("Exciting port2");
	// TODO
}

bool VNA::Init() {
	Si5351.Init();
	Source.Init();
	LO1.Init();

	// Use Si5351 to generate referene frequencies for other PLLs and ADC
	Si5351.SetPLL(Si5351C::PLL::A, 900000000, Si5351C::PLLSource::XTAL);
	// Both MAX2871 get a 100MHz reference
	Si5351.SetCLK(0, 100000000, Si5351C::PLL::A);
	Si5351.Enable(0);
	Si5351.SetCLK(1, 100000000, Si5351C::PLL::A);
	Si5351.Enable(1);
	// 10 MHz clock for ADC
	Si5351.SetCLK(2, 10000000, Si5351C::PLL::A);
	Si5351.Enable(2);

	// Generate second LO with Si5351
	Si5351.SetCLK(3, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(3);
	Si5351.SetCLK(4, IF1 - IF2, Si5351C::PLL::A);
	Si5351.Enable(4);

	// Configure the reference of both MAX2871
	Source.SetReference(100000000, false, 1, false);
	Source.Update();
	LO1.SetReference(100000000, false, 1, false);
	LO1.Update();

	LOG_INFO("Initialized");
	return true;
}

static VNA::Callback callback;
static VNA::Result result;
static uint8_t activePort;
static uint16_t samples;

static void SamplingCallback(Sampling::Result res) {
	// Calculate magnitude and phase relative to reference
	float Mag1, Phase1, Mag2, Phase2;
	Mag1 = abs(res.Port1) / abs(res.Ref);
	Phase1 = arg(res.Port1) / arg(res.Ref);
	Mag2 = abs(res.Port2) / abs(res.Ref);
	Phase2 = arg(res.Port2) / arg(res.Ref);
	if (activePort == 1) {
		activePort = 2;
		ExcitatePort2();
		Delay::ms(1);
		Sampling::Start(SamplingCallback, samples);
		result.S11Mag = Mag1;
		result.S11Phase = Phase1;
		result.S21Mag = Mag2;
		result.S21Phase = Phase2;
	} else {
		result.S12Mag = Mag1;
		result.S12Phase = Phase1;
		result.S22Mag = Mag2;
		result.S22Phase = Phase2;
		LOG_DEBUG("Result:");
		LOG_DEBUG("S11: %f@%f", result.S11Mag, result.S11Phase * 180 / M_PI);
		LOG_DEBUG("S21: %f@%f", result.S21Mag, result.S21Phase * 180 / M_PI);
		LOG_DEBUG("S12: %f@%f", result.S12Mag, result.S12Phase * 180 / M_PI);
		LOG_DEBUG("S22: %f@%f", result.S22Mag, result.S22Phase * 180 / M_PI);
		if (callback) {
			callback(result);
		}
	}
}

bool VNA::Measure(uint64_t f, Callback cb, uint16_t nsamples) {
	LOG_INFO("Starting measurement at f=%lu%06luHz", (uint32_t ) (f / 1000000),
			(uint32_t ) (f % 1000000));
	callback = cb;
	Source.SetFrequency(f);
	Source.Update();
	LO1.SetFrequency(f + IF1);
	LO1.Update();
	ExcitatePort1();
	activePort = 1;
	Delay::ms(1);
	samples = nsamples;
	Sampling::Start(SamplingCallback, samples);
	return true;
}
