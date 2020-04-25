#include "Sampling.hpp"

#include "stm.hpp"

#define LOG_LEVEL	LOG_LEVEL_WARN
#define LOG_MODULE	"Sampling"
#include "Log.h"

extern ADC_HandleTypeDef hadc1, hadc2, hadc3;
extern TIM_HandleTypeDef htim4;
static uint16_t sampleBuffer[3 * Sampling::Buffer];
static uint32_t nsamples;

static Sampling::Callback callback;
static uint32_t starttime;

static int32_t refI = 0;
static int32_t refQ = 0;
static int32_t port1I = 0;
static int32_t port1Q = 0;
static int32_t port2I = 0;
static int32_t port2Q = 0;
static uint32_t samples_cnt;
static bool active;
static bool firstISR;

bool Sampling::Init() {
	active = false;
	HAL_ADC_Start(&hadc2);
	HAL_ADC_Start(&hadc3);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*) sampleBuffer, 3 * Sampling::Buffer);
	HAL_TIM_Base_Start(&htim4);
	return true;
}

bool Sampling::Start(Callback cb, uint16_t samples) {
	LOG_INFO("Started");
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
	firstISR = true;
	active = true;
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	return true;
}

static void ComputeComplex(uint16_t *bufStart) {
	if(!active) {
		return;
	}
	if(firstISR) {
		// ignore first batch of samples
		firstISR = false;
		return;
	}
	constexpr uint32_t bufsize = 3 * Sampling::Buffer / 2;
	for (uint32_t i = 0; i < bufsize; i += 12) {
		refI += bufStart[i] - bufStart[i + 6];
		refQ += bufStart[i + 3] - bufStart[i + 9];
		port1I += bufStart[i + 1] - bufStart[i + 7];
		port1Q += bufStart[i + 4] - bufStart[i + 10];
		port2I += bufStart[i + 2] - bufStart[i + 8];
		port2Q += bufStart[i + 5] - bufStart[i + 11];
		samples_cnt += 4;
		if(samples_cnt >= nsamples) {
			Sampling::Result res;
			res.Ref = std::complex<float>((float) refI / (nsamples / 2),
					(float) refQ / (nsamples / 2));
			res.Port1 = std::complex<float>((float) port1I / (nsamples / 2),
					(float) port1Q / (nsamples / 2));
			res.Port2 = std::complex<float>((float) port2I / (nsamples / 2),
					(float) port2Q / (nsamples / 2));
			uint32_t duration = HAL_GetTick() - starttime;
			LOG_INFO("Stopped, took %lums", duration);
			LOG_DEBUG("Result: Ref: %f/%f, Port1: %f/%f, Port2: %f/%f", real(res.Ref),
					imag(res.Ref), real(res.Port1), imag(res.Port1), real(res.Port2),
					imag(res.Port2));
			active = false;
			if (callback) {
				callback(res);
			}
			break;
		}
	}
}

extern "C" {
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc == &hadc1) {
		ComputeComplex(&sampleBuffer[3 * Sampling::Buffer / 2]);
	}
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc == &hadc1) {
		ComputeComplex(&sampleBuffer[0]);
	}
}
}
