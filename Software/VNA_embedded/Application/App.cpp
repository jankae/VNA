#include <VNA.hpp>
#include "App.h"

#include "delay.hpp"
#include "Communication.h"
#include "main.h"
#include "Exti.hpp"
#include "FPGA.hpp"
#include <complex>

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
	// Allow USB enumeration
	USB_EN_GPIO_Port->BSRR = USB_EN_Pin;

//	Protocol::SweepSettings s;
//	s.f_start = 1000000000;
//	s.f_stop = 1000000000;
//	s.if_bandwidth = 1000;
//	s.mdbm_excitation = 0;
//	s.points = 1;
//	VNA::ConfigureSweep(s);

	uint32_t lastNewPoint = HAL_GetTick();
	bool sweepActive = false;

	while (1) {
		if (newResult) {
			Protocol::PacketInfo packet;
			packet.type = Protocol::PacketType::Datapoint;
			packet.datapoint = result;
			Communication::Send(packet);
			newResult = false;
			lastNewPoint = HAL_GetTick();
		}
		if (newSettings) {
			LOG_INFO("New settings received");
			newSettings = false;
			VNA::ConfigureSweep(settings);
			sweepActive = true;
			lastNewPoint = HAL_GetTick();
		}
		if(sweepActive && HAL_GetTick() - lastNewPoint > 200) {
			LOG_WARN("Timed out waiting for point, last received point was %d", result.pointNum);
			LOG_WARN("FPGA status: 0x%04x", FPGA::GetStatus());
//			// restart the current sweep
			VNA::ConfigureSweep(settings);
			sweepActive = true;
			lastNewPoint = HAL_GetTick();
		}
	}
}

void App::NewSettings(Protocol::SweepSettings s) {
	settings = s;
	newSettings = true;
}
