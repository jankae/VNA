#include "Sampling.hpp"

#include "stm.hpp"

#define LOG_LEVEL	LOG_LEVEL_DEBUG
#define LOG_MODULE	"Sampling"
#include "Log.h"

extern ADC_HandleTypeDef hadc1, hadc2, hadc3;
extern TIM_HandleTypeDef htim4;
static uint16_t sampleBuffer[3][Sampling::MaxSamples];
static uint16_t nsamples;

static Sampling::Callback callback;

bool Sampling::Start(Callback cb, uint16_t samples) {
	callback = cb;
	nsamples = samples;
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) sampleBuffer[0], samples);
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) sampleBuffer[1], samples);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*) sampleBuffer[2], samples);
	HAL_TIM_Base_Start(&htim4);
	LOG_INFO("Started");
	return true;
}

static void ComputeComplex() {
	LOG_INFO("Stopped");
	HAL_TIM_Base_Stop(&htim4);
	int32_t refI = 0;
	int32_t refQ = 0;
	int32_t port1I = 0;
	int32_t port1Q = 0;
	int32_t port2I = 0;
	int32_t port2Q = 0;
	for (uint16_t i = 0; i < nsamples; i += 4) {
		refI += sampleBuffer[0][i] - sampleBuffer[0][i + 2];
		refQ += sampleBuffer[0][i + 1] - sampleBuffer[0][i + 3];
		port1I += sampleBuffer[1][i] - sampleBuffer[1][i + 2];
		port1Q += sampleBuffer[1][i + 1] - sampleBuffer[1][i + 3];
		port2I += sampleBuffer[2][i] - sampleBuffer[2][i + 2];
		port2Q += sampleBuffer[2][i + 1] - sampleBuffer[2][i + 3];
	}
	Sampling::Result res;
	res.Ref = std::complex<float>((float) refI / (nsamples / 2),
			(float) refQ / (nsamples / 2));
	res.Port1 = std::complex<float>((float) port1I / (nsamples / 2),
			(float) port1Q / (nsamples / 2));
	res.Port2 = std::complex<float>((float) port2I / (nsamples / 2),
			(float) port2Q / (nsamples / 2));
	LOG_DEBUG("Result: Ref: %f/%f, Port1: %f/%f, Port2: %f/%f", real(res.Ref),
			imag(res.Ref), real(res.Port1), imag(res.Port1), real(res.Port2),
			imag(res.Port2));
	if (callback) {
		callback(res);
	}
}

extern "C" {
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if(hadc == &hadc3) {
		ComputeComplex();
	}
}
}
