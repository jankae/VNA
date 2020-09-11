#pragma once

#include "stm.hpp"
#include "main.h"

#define FPGA_SPI			hspi3
#define CONFIGURATION_SPI	hspi3
extern SPI_HandleTypeDef FPGA_SPI, CONFIGURATION_SPI;

namespace FPGAHAL {

using GPIO = struct {
	GPIO_TypeDef *gpio;
	uint16_t pin;
};
static constexpr GPIO CS = {.gpio = FPGA_CS_GPIO_Port, .pin = FPGA_CS_Pin};
static constexpr GPIO PROGRAM_B = {.gpio = nullptr, .pin = 0};
static constexpr GPIO INIT_B = {.gpio = nullptr, .pin = 0};
static constexpr GPIO DONE = {.gpio = nullptr, .pin = 0};
static constexpr GPIO FPGA_RESET = {.gpio = FPGA_RESET_GPIO_Port, .pin = FPGA_RESET_Pin};
static constexpr GPIO AUX1 = {.gpio = FPGA_AUX1_GPIO_Port, .pin = FPGA_AUX1_Pin};
static constexpr GPIO AUX2 = {.gpio = FPGA_AUX2_GPIO_Port, .pin = FPGA_AUX2_Pin};
static constexpr GPIO AUX3 = {.gpio = FPGA_AUX3_GPIO_Port, .pin = FPGA_AUX3_Pin};

static inline void Low(GPIO g) {
	if(g.gpio) {
		g.gpio->BSRR = g.pin << 16;
	}
}
static inline void High(GPIO g) {
	if(g.gpio) {
		g.gpio->BSRR = g.pin;
	}
}
bool isHigh(GPIO g) {
	if(g.gpio) {
		return g.gpio->IDR & g.pin;
	} else {
		return false;
	}
}

}
