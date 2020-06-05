#include "Protocol.hpp"

#include <cstring>

/*
 * General packet format:
 * 1. 1 byte header
 * 2. 2 byte overall packet length (with header and checksum)
 * 3. packet type
 * 4. packet payload
 * 5. 4 byte CRC32 (with header)
 */

static constexpr uint8_t header = 0x5A;
static constexpr uint8_t header_size = 4;

#define CRC32_POLYGON 0xEDB88320
uint32_t CRC32(uint32_t crc, const void *data, uint32_t len) {
	uint8_t *u8buf = (uint8_t*) data;
	int k;

	crc = ~crc;
	while (len--) {
		crc ^= *u8buf++;
		for (k = 0; k < 8; k++)
			crc = crc & 1 ? (crc >> 1) ^ CRC32_POLYGON : crc >> 1;
	}
	return ~crc;
}

// TODO proper encoding/decoding tolerant of endianness and padding
static uint16_t EncodeDatapoint(Protocol::Datapoint d, uint8_t *buf,
		uint16_t bufSize) {
	if (bufSize < sizeof(d)) {
		return 0;
	} else {
		memcpy(buf, &d, sizeof(d));
		return sizeof(d);
	}
}
static uint16_t EncodeSweepSettings(Protocol::SweepSettings d, uint8_t *buf,
		uint16_t bufSize) {
	if (bufSize < sizeof(d)) {
		return 0;
	} else {
		memcpy(buf, &d, sizeof(d));
		return sizeof(d);
	}
}
static uint16_t EncodeStatus(Protocol::Status d, uint8_t *buf,
                                     uint16_t bufSize) {
    if (bufSize < sizeof(d)) {
        return 0;
    } else {
        memcpy(buf, &d, sizeof(d));
        return sizeof(d);
    }
}
static uint16_t EncodeManualControl(Protocol::ManualControl d, uint8_t *buf,
                                                   uint16_t bufSize) {
    if (bufSize < sizeof(d)) {
        return 0;
    } else {
        memcpy(buf, &d, sizeof(d));
        return sizeof(d);
    }
}
static Protocol::Datapoint DecodeDatapoint(uint8_t *buf) {
	Protocol::Datapoint d;
	memcpy(&d, buf, sizeof(d));
	return d;
}
static Protocol::SweepSettings DecodeSweepSettings(uint8_t *buf) {
	Protocol::SweepSettings d;
	memcpy(&d, buf, sizeof(d));
	return d;
}
static Protocol::Status DecodeStatus(uint8_t *buf) {
    Protocol::Status d;
    memcpy(&d, buf, sizeof(d));
    return d;
}
static Protocol::ManualControl DecodeManualControl(uint8_t *buf) {
    Protocol::ManualControl d;
    memcpy(&d, buf, sizeof(d));
    return d;
}

uint16_t Protocol::DecodeBuffer(uint8_t *buf, uint16_t len, PacketInfo *info) {
    if (!info || !len) {
        info->type = PacketType::None;
		return 0;
	}
	uint8_t *data = buf;
	/* Remove any out-of-order bytes in front of the frame */
	while (*data != header) {
		data++;
		if(--len == 0) {
			/* Reached end of data */
			/* No frame contained in data */
			info->type = PacketType::None;
			return data - buf;
		}
	}
	/* At this point, data points to the beginning of the frame */
	if(len < header_size) {
		/* the frame header has not been completely received */
		info->type = PacketType::None;
		return data - buf;
	}

	/* Evaluate frame size */
	uint16_t length = *(uint16_t*) &data[1];
	if(len < length) {
		/* The frame payload has not been completely received */
		info->type = PacketType::None;
		return data - buf;
	}

	/* The complete frame has been received, check checksum */
    uint32_t crc = *(uint32_t*) &data[length - 4];
	uint32_t compare = CRC32(0, data, length - 4);
	if(crc != compare) {
		// CRC mismatch, remove header
		data += 1;
		info->type = PacketType::None;
		return data - buf;
	}

	// Valid packet, extract packet info
	info->type = (PacketType) data[3];
	switch (info->type) {
	case PacketType::Datapoint:
		info->datapoint = DecodeDatapoint(&data[4]);
		break;
	case PacketType::SweepSettings:
		info->settings = DecodeSweepSettings(&data[4]);
		break;
    case PacketType::Status:
        info->status = DecodeStatus(&data[4]);
        break;
    case PacketType::ManualControl:
        info->manual = DecodeManualControl(&data[4]);
        break;
	}

	return data - buf + length;
}

uint16_t Protocol::EncodePacket(PacketInfo packet, uint8_t *dest, uint16_t destsize) {
	uint16_t payload_size = 0;
	switch (packet.type) {
	case PacketType::Datapoint:
		payload_size = EncodeDatapoint(packet.datapoint, &dest[4], destsize - 4);
		break;
	case PacketType::SweepSettings:
		payload_size = EncodeSweepSettings(packet.settings, &dest[4], destsize - 4);
		break;
    case PacketType::Status:
        payload_size = EncodeStatus(packet.status, &dest[4], destsize - 4);
        break;
    case PacketType::ManualControl:
        payload_size = EncodeManualControl(packet.manual, &dest[4], destsize - 4);
        break;
    }
	if (payload_size == 0 || payload_size + 8 > destsize) {
		// encoding failed, buffer too small
		return 0;
	}
	// Write header
	dest[0] = header;
	uint16_t overall_size = payload_size + 8;
	memcpy(&dest[1], &overall_size, 2);
	dest[3] = (int) packet.type;
	// Calculate checksum
	uint32_t crc = CRC32(0, dest, overall_size - 4);
	memcpy(&dest[overall_size - 4], &crc, 4);
	return overall_size;
}
