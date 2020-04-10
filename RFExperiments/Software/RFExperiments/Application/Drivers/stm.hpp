#pragma once

#include "stm32f746xx.h"
#include "stm32f7xx_hal.h"

namespace STM {

static inline bool InInterrupt() {
	return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

}
