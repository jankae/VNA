#pragma once

#include <cstdint>

namespace Protocol {

using Datapoint = struct _datapoint {
	float real_S11, imag_S11;
	float real_S21, imag_S21;
	float real_S12, imag_S12;
	float real_S22, imag_S22;
	uint64_t frequency;
	uint16_t pointNum;
} __attribute__((packed));

using SweepSettings = struct _sweepSettings {
	uint64_t f_start;
	uint64_t f_stop;
	uint16_t points;
    uint32_t if_bandwidth;
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
