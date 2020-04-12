#pragma once

#include <cstdint>

namespace Protocol {

using Datapoint = struct _datapoint {
	float S11Mag, S11Phase;
	float S21Mag, S21Phase;
	float S12Mag, S12Phase;
	float S22Mag, S22Phase;
	uint64_t frequency;
	uint16_t pointNum;
} __attribute__((packed));

using SweepSettings = struct _sweepSettings {
	uint64_t f_start;
	uint64_t f_stop;
	uint16_t points;
	uint16_t averaging;
	int16_t mdbm_excitation;
} __attribute__((packed));

enum class PacketType : uint8_t {
	None,
	Datapoint,
	SweepSettings,
};

using PacketInfo = struct _packetinfo {
	PacketType type;
	union {
		Datapoint datapoint;
		SweepSettings settings;
	};
};

uint16_t DecodeBuffer(uint8_t *buf, uint16_t len, PacketInfo *info);
uint16_t EncodePacket(PacketInfo packet, uint8_t *dest, uint16_t destsize);

}
