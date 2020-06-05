#include <VNA.hpp>
#include "App.h"

#include "delay.hpp"
#include "Communication.h"
#include "main.h"
#include "Exti.hpp"
#include "FPGA.hpp"
#include <complex>
#include <cstring>

#define LOG_LEVEL	LOG_LEVEL_INFO
#define LOG_MODULE	"App"
#include "Log.h"

static volatile bool newResult;
static Protocol::Datapoint result;
static volatile bool newSettings = false;
static Protocol::SweepSettings settings;

static volatile bool newStatus = false;
static FPGA::SamplingResult statusResult;
static volatile bool newManual = false;
static Protocol::ManualControl manual;

void VNACallback(Protocol::Datapoint res) {
	result = res;
	newResult = true;
}
void VNAStatusCallback(FPGA::SamplingResult res) {
	statusResult = res;
	newStatus = true;
}

void App_Start() {
	Log_Init();
	LOG_INFO("Start");
	Exti::Init();
	if (!VNA::Init()) {
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
			if(result.pointNum == settings.points - 1) {
				// end of sweep
				// read PLL temperatures
				uint8_t tempSource, tempLO;
				VNA::GetTemps(&tempSource, &tempLO);
				LOG_INFO("PLL temperatures: %u/%u", tempSource, tempLO);
				// Read ADC min/max
				auto limits = FPGA::GetADCLimits();
				FPGA::ResetADCLimits();
				LOG_INFO("ADC limits: P1: %d/%d P2: %d/%d R: %d/%d",
						limits.P1min, limits.P1max, limits.P2min, limits.P2max,
						limits.Rmin, limits.Rmax);
				// Start next sweep
				FPGA::StartSweep();
			}
		}
		if (newSettings) {
			LOG_INFO("New settings received");
			newSettings = false;
			VNA::ConfigureSweep(settings, VNACallback);
			sweepActive = true;
			lastNewPoint = HAL_GetTick();
		}
		if(sweepActive && HAL_GetTick() - lastNewPoint > 1000) {
			LOG_WARN("Timed out waiting for point, last received point was %d", result.pointNum);
			LOG_WARN("FPGA status: 0x%04x", FPGA::GetStatus());
//			// restart the current sweep
			VNA::ConfigureSweep(settings, VNACallback);
			sweepActive = true;
			lastNewPoint = HAL_GetTick();
		}
		if (newManual) {
			LOG_INFO("New manual control");
			sweepActive = false;
			newManual = false;
			VNA::ConfigureManual(manual, VNAStatusCallback);
		}
		if (newStatus) {
			newStatus = false;
			Protocol::PacketInfo p;
			p.type = Protocol::PacketType::Status;
			memset(&p.status, 0, sizeof(p.status));
			uint16_t isr_flags = FPGA::GetStatus();
			if (!(isr_flags & 0x0002)) {
				p.status.source_locked = 1;
			}
			if (!(isr_flags & 0x0001)) {
				p.status.LO_locked = 1;
			}
			auto limits = FPGA::GetADCLimits();
			FPGA::ResetADCLimits();
			p.status.port1min = limits.P1min;
			p.status.port1max = limits.P1max;
			p.status.port2min = limits.P2min;
			p.status.port2max = limits.P2max;
			p.status.refmin = limits.Rmin;
			p.status.refmax = limits.Rmax;
			p.status.port1real = (float) statusResult.P1I / manual.Samples;
			p.status.port1imag = (float) statusResult.P1Q / manual.Samples;
			p.status.port2real = (float) statusResult.P2I / manual.Samples;
			p.status.port2imag = (float) statusResult.P2Q / manual.Samples;
			p.status.refreal = (float) statusResult.RefI / manual.Samples;
			p.status.refimag = (float) statusResult.RefQ / manual.Samples;
			VNA::GetTemps(&p.status.temp_source, &p.status.temp_LO);
			Communication::Send(p);
			// Trigger next status update
			FPGA::StartSweep();
		}
	}
}

void App::NewSettings(Protocol::SweepSettings s) {
	settings = s;
	newSettings = true;
}

void App::SetManual(Protocol::ManualControl m) {
	manual = m;
	newManual = true;
}
