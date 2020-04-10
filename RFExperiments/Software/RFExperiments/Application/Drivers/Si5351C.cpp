#include "Si5351C.hpp"

bool Si5351C::Init(uint32_t clkin_freq) {
	bool success = true;

	uint8_t clkinDiv = 0;
	while (clkin_freq / (1 << clkinDiv) > 40000000) {
		if (clkinDiv < 3) {
			clkinDiv++;
		} else {
			return false;
		}
	}
	FreqCLKINDiv = clkin_freq / (1 << clkinDiv);
	// Write CLK in divider
	uint8_t value;
	success &= ReadRegister(Reg::PLLInputSource, &value);
	value &= ~0xC0;
	value |= clkinDiv << 6;
	success &= WriteRegister(Reg::PLLInputSource, value);

	// Disable OEB pin functionality
	success &= WriteRegister(Reg::OEBPinMask, 0xFF);
	// Disable all outputs
	success &= WriteRegister(Reg::OutputEnableControl, 0xFF);

	return success;
}

bool Si5351C::SetPLL(PLL pll, uint32_t frequency, PLLSource src) {
	if (frequency < 600000000 || frequency > 900000000) {
		return false;
	}
	PLLConfig c;
	c.IntegerMode = false;
	c.source = src;

	uint32_t srcFreq = src == PLLSource::XTAL ? FreqXTAL : FreqCLKINDiv;
	// see https://www.silabs.com/documents/public/application-notes/AN619.pdf (page 3)
	uint32_t div20 = srcFreq * (1UL << 20) / frequency;
	// Check for valid range
	if (div20 < 15 * (1UL << 20) || div20 > 90 * (1UL << 20)) {
		return false;
	}
	// Always use highest available c
	c.P3 = 0xFFFFF;
	// highest 18 bits of divider with offset
	c.P1 = ((div20 >> 14) & 0x3FFFF) - 512;
	// lower 14 bit left shifted to 20bit border
	c.P2 = (div20 & 0x3FFF) << 6;

	FreqPLL[(int) pll] = frequency;
	return WritePLLConfig(c, pll);
}

bool Si5351C::SetCLK(uint8_t clknum, uint32_t frequency, PLL source) {
	ClkConfig c;
	c.DivideBy4 = false;
	c.IntegerMode = false;
	c.Inverted = false;
	c.PoweredDown = false;
	c.RDiv = 1;
	c.source = source;
	c.strength = DriveStrength::mA8;

	uint32_t pllFreq = FreqPLL[(int) source];
	if (clknum > 5) {
		// outputs 6 and 7 are integer dividers only
		uint32_t div = pllFreq / frequency;
		if (div > 254 || div < 6) {
			return false;
		}
		c.P1 = div;
	} else {
		ClkConfig c;
		while (pllFreq / (frequency * c.RDiv) >= 2048
				|| (frequency * c.RDiv) < 500000) {
			if (c.RDiv < 128) {
				c.RDiv *= 2;
			} else {
				return false;
			}
		}
		// see https://www.silabs.com/documents/public/application-notes/AN619.pdf (page 6)
		uint32_t div20 = pllFreq * (1UL << 20) / (frequency * c.RDiv);
		// Always use highest available c
		c.P3 = 0xFFFFF;
		// highest 18 bits of divider with offset
		c.P1 = ((div20 >> 14) & 0x3FFFF) - 512;
		// lower 14 bit left shifted to 20bit border
		c.P2 = (div20 & 0x3FFF) << 6;
	}
	return WriteClkConfig(c, clknum);
}

bool Si5351C::Enable(uint8_t clknum) {
	return ClearBits(Reg::OutputEnableControl, 1 << clknum);
}

bool Si5351C::Disable(uint8_t clknum) {
	return SetBits(Reg::OutputEnableControl, 1 << clknum);
}

bool Si5351C::WritePLLConfig(PLLConfig config, PLL pll) {
	uint8_t PllData[8];
	// See register map in https://www.silabs.com/documents/public/application-notes/AN619.pdf (page 11)
	PllData[0] = (config.P3 >> 8) & 0xFF;
	PllData[1] = config.P3 & 0xFF;
	PllData[2] = (config.P1 >> 16) & 0x03;
	PllData[3] = (config.P1 >> 8) & 0xFF;
	PllData[4] = config.P1 & 0xFF;
	PllData[5] = ((config.P3 >> 12) & 0xF0) | ((config.P2 >> 16) & 0x0F);
	PllData[6] = (config.P2 >> 8) & 0xFF;
	PllData[7] = config.P2 & 0xFF;
	bool success = true;
	Reg reg = pll == PLL::A ? Reg::MSNA_CONFIG : Reg::MSNB_CONFIG;
	success &= WriteRegisterRange(reg, PllData, sizeof(PllData));
	reg = pll == PLL::A ? Reg::CLK6Control : Reg::CLK7Control;
	if (config.IntegerMode) {
		success &=SetBits(reg, 0x40);
	} else {
		success &=ClearBits(reg, 0x40);
	}
	uint8_t mask = pll == PLL::A ? 0x40 : 0x80;
	if (config.source == PLLSource::XTAL) {
		success &=ClearBits(Reg::PLLInputSource, mask);
	} else {
		success &=SetBits(Reg::PLLInputSource, mask);
	}

	// Reset the PLL
	mask = pll == PLL::A ? 0x20 : 0x80;
	success &=SetBits(Reg::PLLReset, mask);

	return success;
}

bool Si5351C::WriteClkConfig(ClkConfig config, uint8_t clknum) {
	bool success = true;
	// compile CLKControl register
	uint8_t clkcontrol = 0x0C;
	if (config.PoweredDown) {
		clkcontrol |= 0x80;
	}
	if (clknum <= 5) {
		if (config.IntegerMode) {
			clkcontrol |= 0x40;
		}
	} else {
		// preserve bit 6
		uint8_t value = 0x00;
		success &= ReadRegister((Reg) ((int) Reg::CLK0Control + clknum),
				&value);
		clkcontrol |= value & 0x40;
	}
	if (config.source == PLL::B) {
		clkcontrol |= 0x20;
	}
	if (config.Inverted) {
		clkcontrol |= 0x10;
	}
	switch (config.strength) {
	case DriveStrength::mA2:
		break;
	case DriveStrength::mA4:
		clkcontrol |= 0x01;
		break;
	case DriveStrength::mA6:
		clkcontrol |= 0x02;
		break;
	case DriveStrength::mA8:
		clkcontrol |= 0x03;
		break;
	}
	Reg reg = (Reg) ((int) Reg::CLK0Control + clknum);
	success &= WriteRegister(reg, clkcontrol);
	if (clknum <= 5) {
		uint8_t ClkData[8];
		ClkData[0] = (config.P3 >> 8) & 0xFF;
		ClkData[1] = config.P3 & 0xFF;
		ClkData[2] = (31 - __builtin_clz(config.RDiv)) << 4
				| (config.DivideBy4 ? 0xC0 : 0x00) | ((config.P1 >> 16) & 0x03);
		ClkData[3] = (config.P1 >> 8) & 0xFF;
		ClkData[4] = config.P1 & 0xFF;
		ClkData[5] = ((config.P3 >> 12) & 0xF0) | ((config.P2 >> 16) & 0x0F);
		ClkData[6] = (config.P2 >> 8) & 0xFF;
		ClkData[7] = config.P2 & 0xFF;
		// Calculate address of register control block
		reg = (Reg) ((int) Reg::MS0_CONFIG + 8 * clknum);
		success &= WriteRegisterRange(reg, ClkData, sizeof(ClkData));
	} else if (clknum == 6) {
		success &= WriteRegister(Reg::MS6_CONFIG, config.P1 & 0xFF);
	} else {
		success &= WriteRegister(Reg::MS7_CONFIG, config.P1 & 0xFF);
	}

	return success;
}

bool Si5351C::WriteRegister(Reg reg, uint8_t data) {
	return WriteRegisterRange(reg, &data, 1);
}

bool Si5351C::ReadRegister(Reg reg, uint8_t *data) {
	return HAL_I2C_Mem_Read(i2c, address, (int) reg,
	I2C_MEMADD_SIZE_8BIT, data, 1, 100) == HAL_OK;
}

bool Si5351C::SetBits(Reg reg, uint8_t bits) {
	uint8_t value = 0;
	if (!ReadRegister(reg, &value)) {
		return false;
	}
	value |= bits;
	return WriteRegister(reg, value);
}

bool Si5351C::ClearBits(Reg reg, uint8_t bits) {
	uint8_t value = 0;
	if (!ReadRegister(reg, &value)) {
		return false;
	}
	value &= ~bits;
	return WriteRegister(reg, value);
}

bool Si5351C::WriteRegisterRange(Reg start, uint8_t *data, uint8_t len) {
	return HAL_I2C_Mem_Write(i2c, address, (int) start,
	I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK;
}
