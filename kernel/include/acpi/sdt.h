#ifndef __SDT_H_INCLUDED__
#define __SDT_H_INCLUDED__

#include <utils.h>

struct sdt_header {
	char sig[4];
	uint32_t len;
	uint8_t revision;
	uint8_t checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
} PACKED;

inline static bool sdt_verify(struct sdt_header *header) {
	uint8_t val = 0;
	for (uint32_t i = 0; i < header->len; ++i) {
		val += ((uint8_t *)header)[i];
	}
	return val == 0;
}

#endif