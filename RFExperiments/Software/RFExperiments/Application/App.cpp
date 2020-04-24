#include "App.h"

#include "VNA.hpp"
#include "delay.hpp"
#include "Communication.h"

#define LOG_LEVEL	LOG_LEVEL_INFO
#define LOG_MODULE	"App"
#include "Log.h"

static bool newSettings = false;
static Protocol::SweepSettings settings;
uint16_t pointCnt = 0;

static bool done;
static VNA::Result result;
void VNACallback(VNA::Result res) {
	result = res;
	done = true;
}

void App_Start() {
	Log_Init();
	LOG_INFO("Start");
	VNA::Init();
	// Wait for initial settings before starting sweep
	while (!newSettings);
	LOG_INFO("New settings received");
	pointCnt = 0;
	newSettings = false;
	while (1) {
		uint64_t freq = settings.f_start
				+ (settings.f_stop - settings.f_start) * pointCnt
						/ (settings.points - 1);
		LOG_INFO("Setting up for point %u/%u: %lu%06luHz", pointCnt + 1,
				settings.points, (uint32_t ) (freq / 1000000),
				(uint32_t ) (freq % 1000000));
		uint16_t averages = 1000000 / settings.if_bandwidth;
		// round up to next multiple of 4
		averages = (averages + 3) & ~0x03;
		VNA::Measure(freq, VNACallback, averages);
		while (!done && !newSettings) {
		}
		if (done) {
			LOG_DEBUG("Measurement done");
			Protocol::PacketInfo packet;
			packet.type = Protocol::PacketType::Datapoint;
			packet.datapoint.real_S11 = real(result.S11);
			packet.datapoint.imag_S11 = imag(result.S11);
			packet.datapoint.real_S21 = real(result.S21);
			packet.datapoint.imag_S21 = imag(result.S21);
			packet.datapoint.real_S12 = real(result.S12);
			packet.datapoint.imag_S12 = imag(result.S12);
			packet.datapoint.real_S22 = real(result.S22);
			packet.datapoint.imag_S22 = imag(result.S22);
			packet.datapoint.frequency = freq;
			packet.datapoint.pointNum = pointCnt;
			Communication::Send(packet);
			done = false;
			pointCnt++;
			if (pointCnt >= settings.points) {
				pointCnt = 0;
			}
		}
		if (newSettings) {
			LOG_INFO("New settings received");
			pointCnt = 0;
			newSettings = false;
		}
	}
}

void App::NewSettings(Protocol::SweepSettings s) {
	settings = s;
	newSettings = true;
}
