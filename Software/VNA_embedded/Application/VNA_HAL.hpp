#pragma once

#include "stm.hpp"
#include "Si5351C.hpp"
#include "max2871.hpp"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi3;

namespace VNAHAL {

static Si5351C Si5351 = Si5351C(&hi2c1, 26000000);
static MAX2871 Source = MAX2871(&hspi3, FPGA_CS_GPIO_Port, FPGA_CS_Pin, nullptr, 0, nullptr, 0, nullptr, 0, GPIOB, GPIO_PIN_4);
static MAX2871 LO1 = MAX2871(&hspi3, FPGA_CS_GPIO_Port, FPGA_CS_Pin, nullptr, 0, nullptr, 0, nullptr, 0, GPIOB, GPIO_PIN_4);

// Mapping of the Si5351 channels to PLLs/Mixers
namespace SiChannel {
	enum {
		Source = 3,
		LO1 = 2,
		Port2LO2 = 1,
		RefLO2 = 5,
		Port1LO2 = 4,
		LowbandSource = 0,
		ReferenceOut = 6,
		FPGA = 7,
	};
}

}
