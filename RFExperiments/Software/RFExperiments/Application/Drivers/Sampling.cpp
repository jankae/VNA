#include "Sampling.hpp"

#include "stm.hpp"
#include <cmath>
#include "main.h"

#define LOG_LEVEL	LOG_LEVEL_INFO
#define LOG_MODULE	"Sampling"
#include "Log.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

constexpr uint16_t CalcPeriodic() {
	constexpr double SampleRate = Sampling::ADCClock / Sampling::ADCDivider;
	constexpr double SamplesPerPeriod = SampleRate / 250000;
	double Samples = SamplesPerPeriod;
	while (Samples - floor(Samples) > 0.000001) {
		Samples += SamplesPerPeriod;
	}
	uint16_t SampleStep = Samples;
	return SampleStep;
}

static constexpr uint16_t PeriodicBufferSize = CalcPeriodic();

constexpr uint16_t CalcBufferSize() {
	constexpr double SampleRate = Sampling::ADCClock / Sampling::ADCDivider;
	// At least 0.1ms between ADC DMA interrupts
	uint16_t MinSamples = SampleRate / 15000;
	// Buffer also has to fit an integer number of periods
	if (MinSamples % PeriodicBufferSize) {
		MinSamples += PeriodicBufferSize - MinSamples % PeriodicBufferSize;
	}
	return MinSamples;
}

static constexpr uint16_t BufferSize = CalcBufferSize();

struct FFTCoefficients {
	constexpr FFTCoefficients() : I(), Q() {
		constexpr double SampleRate = Sampling::ADCClock / Sampling::ADCDivider;
		constexpr double SamplesPerPeriod = SampleRate / 250000;
		for(uint16_t i=0;i<PeriodicBufferSize;i++) {
			I[i] = cos(i * 2*M_PI / SamplesPerPeriod);
			Q[i] = sin(i * 2*M_PI / SamplesPerPeriod);
		}
	}
	float I[PeriodicBufferSize];
	float Q[PeriodicBufferSize];
};

static constexpr auto fftCoefficients = FFTCoefficients();

extern ADC_HandleTypeDef hadc1, hadc2, hadc3;
extern TIM_HandleTypeDef htim4;
static uint16_t sampleBuffer[3 * 2 * BufferSize]; // 3 ADCs, 2 DMA cycles (half/full)
static uint32_t nsamples;

static Sampling::Callback callback;
static uint32_t starttime;

static float refI = 0;
static float refQ = 0;
static float port1I = 0;
static float port1Q = 0;
static float port2I = 0;
static float port2Q = 0;
static uint32_t samples_cnt;
static volatile bool active;
static uint8_t ignoreCnt;

bool Sampling::Init() {
	LOG_INFO("ADC Clock: %f, ADC Divider: %f -> Buffersize = %u", (float) Sampling::ADCClock, (float) Sampling::ADCDivider, BufferSize);
	for(uint16_t i=0;i<PeriodicBufferSize;i++) {
		LOG_DEBUG("FFT coefficient %d: %f %f", i, fftCoefficients.I[i], fftCoefficients.Q[i]);
	}
	active = false;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_Start(&hadc3);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*) sampleBuffer, 3 * 2 * BufferSize);
	HAL_TIM_OC_Start(&htim4, TIM_CHANNEL_1);
	return true;
}

bool Sampling::Start(Callback cb, uint32_t samples) {
	LOG_DEBUG("Started");
	starttime = HAL_GetTick();
	NVIC_DisableIRQ(DMA2_Stream0_IRQn);
	callback = cb;
	nsamples = samples;
	samples_cnt = 0;
	refI = 0;
	refQ = 0;
	port1I = 0;
	port1Q = 0;
	port2I = 0;
	port2Q = 0;
	ignoreCnt = 2;
	active = true;
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	return true;
}

static void ComputeComplex(uint16_t *bufStart) {
//	TRACE_GPIO_Port->ODR ^= TRACE_Pin;
	if(!active) {
		return;
	}
	if(ignoreCnt) {
		ignoreCnt--;
		// ignore first batches of samples
		return;
	}
	/*
	 * When running at 144MHz, the CPU spends 80% of the time in the DMA interrupt
	 */
	TRACE2_GPIO_Port->BSRR = TRACE2_Pin;
	for (uint32_t i = 0; i < BufferSize; i += PeriodicBufferSize) {
		for (uint16_t j = 0; j < PeriodicBufferSize; j++) {
			refI += bufStart[(i + j) * 3] * fftCoefficients.I[j];
			refQ += bufStart[(i + j) * 3] * fftCoefficients.Q[j];
			port1I += bufStart[(i + j) * 3 + 1] * fftCoefficients.I[j];
			port1Q += bufStart[(i + j) * 3 + 1] * fftCoefficients.Q[j];
			port2I += bufStart[(i + j) * 3 + 2] * fftCoefficients.I[j];
			port2Q += bufStart[(i + j) * 3 + 2] * fftCoefficients.Q[j];
		}
		samples_cnt += PeriodicBufferSize;
		if(samples_cnt >= nsamples) {
			Sampling::Result res;
			res.Ref = std::complex<float>((float) refI / (samples_cnt / 2),
					(float) refQ / (samples_cnt / 2));
			res.Port1 = std::complex<float>((float) port1I / (samples_cnt / 2),
					(float) port1Q / (samples_cnt / 2));
			res.Port2 = std::complex<float>((float) port2I / (samples_cnt / 2),
					(float) port2Q / (samples_cnt / 2));
			uint32_t duration = HAL_GetTick() - starttime;
			LOG_DEBUG("Stopped, took %lums", duration);
			LOG_DEBUG("Result: Ref: %f/%f, Port1: %f/%f, Port2: %f/%f", real(res.Ref),
					imag(res.Ref), real(res.Port1), imag(res.Port1), real(res.Port2),
					imag(res.Port2));
			active = false;
			TRACE2_GPIO_Port->BSRR = TRACE2_Pin << 16;
			if (callback) {
				callback(res);
			}
			return;
		}
	}
	TRACE2_GPIO_Port->BSRR = TRACE2_Pin << 16;
}

extern "C" {
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc == &hadc1) {
		ComputeComplex(&sampleBuffer[3 * BufferSize]);
	}
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc == &hadc1) {
		ComputeComplex(&sampleBuffer[0]);
	}
}
}
