#include "App.h"

#include "VNA.hpp"
#include "delay.hpp"

#define LOG_LEVEL	LOG_LEVEL_DEBUG
#define LOG_MODULE	"App"
#include "Log.h"

static bool done;
static VNA::Result result;
void VNACallback(VNA::Result res) {
	result = res;
	done = true;
}

void App() {
	Log_Init();
	LOG_INFO("Start");
	VNA::Init();
	while (1) {
		VNA::Measure(1000000000, VNACallback, 32768);
		while (!done) {
			Delay::ms(10);
		}
		LOG_INFO("Measurement done");
	}
}
