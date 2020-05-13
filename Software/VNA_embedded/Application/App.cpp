#include <VNA.hpp>
#include "App.h"

#include "delay.hpp"
#include "Communication.h"
#include "main.h"
#include "Exti.hpp"

#define LOG_LEVEL	LOG_LEVEL_INFO
#define LOG_MODULE	"App"
#include "Log.h"

static Protocol::Datapoint result;
static volatile bool newSettings = false;
static Protocol::SweepSettings settings;

static volatile bool newResult;
void VNACallback(Protocol::Datapoint res) {
	result = res;
	newResult = true;
}

void App_Start() {
	Log_Init();
	LOG_INFO("Start");
	Exti::Init();
	if (!VNA::Init(VNACallback)) {
		LOG_CRIT("Initialization failed, unable to start");
		return;
	}

	Protocol::SweepSettings s;
	s.f_start = 1000000000;
	s.f_stop = 1000000000;
	s.if_bandwidth = 10;
	s.mdbm_excitation = 0;
	s.points = 1;
	VNA::ConfigureSweep(s);

	while (1) {
		if (newResult) {
			Protocol::PacketInfo packet;
			packet.type = Protocol::PacketType::Datapoint;
			packet.datapoint = result;
			LOG_INFO("New datapoint S12: %f/%f", result.real_S22, result.imag_S22);
//			Communication::Send(packet);
			newResult = false;
		}
		if (newSettings) {
			LOG_INFO("New settings received");
			newSettings = false;
			VNA::ConfigureSweep(settings);
		}
	}
}

void App::NewSettings(Protocol::SweepSettings s) {
	settings = s;
	newSettings = true;
}
