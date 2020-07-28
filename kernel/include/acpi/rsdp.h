#ifndef __RSDP_H_INCLUDED__
#define __RSDP_H_INCLUDED__

#include <utils.h>

struct rsdpv1 {
	char sig[8];
	uint8_t checksum;
	char oemid[6];
	uint8_t revision;
	uint32_t rsdt;
} PACKED;

struct rsdpv2 {
	struct rsdpv1 base;
	uint32_t length;
	uint64_t xsdt;
	uint8_t extendedChecksum;
	uint8_t reserved[3];
} PACKED;

inline static bool rsdpv1_verify(const struct rsdpv1 *rsdp) {
	const char *bytes = (const char *)rsdp;
	uint8_t sum = 0;
	for (uint64_t i = 0; i < sizeof(struct rsdpv1); ++i) {
		sum += bytes[i];
	}
	return sum == 0;
}

inline static bool rsdpv2_verify(const struct rsdpv2 *rsdp) {
	const char *bytes = (const char *)rsdp;
	uint8_t sum = 0;
	for (uint64_t i = 0; i < sizeof(struct rsdpv2); ++i) {
		sum += bytes[i];
	}
	return sum == 0 && rsdpv1_verify(&(rsdp->base));
}

#endif