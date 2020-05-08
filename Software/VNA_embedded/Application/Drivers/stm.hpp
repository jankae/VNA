#pragma once

#include "stm32l432xx.h"
#include "stm32l4xx_hal.h"

namespace STM {

static inline bool InInterrupt() {
	return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

}
