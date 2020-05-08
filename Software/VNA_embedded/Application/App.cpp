#include <VNA.hpp>
#include "App.h"

#include "delay.hpp"
#include "Communication.h"
#include "main.h"

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
	VNA::Init(VNACallback);
	while (1) {
		if (newResult) {
			Protocol::PacketInfo packet;
			packet.type = Protocol::PacketType::Datapoint;
			packet.datapoint = result;
			Communication::Send(packet);
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
