#include "App.h"

#include "VNA.hpp"
#include "delay.hpp"
#include "Communication.h"

#define LOG_LEVEL	LOG_LEVEL_DEBUG
#define LOG_MODULE	"App"
#include "Log.h"

static bool newSettings = false;
static Protocol::SweepSettings settings = {
		.f_start = 50000000,
		.f_stop = 1000000000,
		.points = 1001,
		.averaging = 32768,
		.mdbm_excitation = 0,
};
uint16_t pointCnt = 0;

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
		uint64_t freq = settings.f_start
				+ (settings.f_stop - settings.f_start) * pointCnt
						/ (settings.points - 1);
		LOG_INFO("Setting up for point %u/%u: %lu%06luHz", pointCnt + 1,
				settings.points, (uint32_t ) (freq / 1000000),
				(uint32_t ) (freq % 1000000));
		VNA::Measure(freq, VNACallback, settings.averaging);
		while (!done && !newSettings) {
		}
		if (done) {
			LOG_DEBUG("Measurement done");
			Protocol::PacketInfo packet;
			packet.type = Protocol::PacketType::Datapoint;
			packet.datapoint.S11Mag = result.S11Mag;
			packet.datapoint.S11Phase = result.S11Phase;
			packet.datapoint.S12Mag = result.S12Mag;
			packet.datapoint.S12Phase = result.S12Phase;
			packet.datapoint.S21Mag = result.S21Mag;
			packet.datapoint.S21Phase = result.S21Phase;
			packet.datapoint.S22Mag = result.S22Mag;
			packet.datapoint.S22Phase = result.S22Phase;
			packet.datapoint.frequency = 1000000000;
			packet.datapoint.pointNum = 0;
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

void NewSettings(Protocol::SweepSettings s) {
	settings = s;
	newSettings = true;
}
