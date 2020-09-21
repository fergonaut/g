// ----------------------------------------------------------------------------
//
// G Library
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#pragma once

#include "gpdu.h"

// ----------------------------------------------------------------------------
// GEthHdr
// ----------------------------------------------------------------------------
#pragma pack(push, 1)
struct G_EXPORT GEthHdr final { // libnet_ethernet_hdr // gilgil temp 2019.05.13
	GMac     dmac_;
	GMac     smac_;
	uint16_t type_;

	GMac     dmac() { return dmac_; }
	GMac     smac() { return smac_; }
	uint16_t type() { return ntohs(type_); }

	// Type
	static const uint16_t Ip4 = 0x0800;
	static const uint16_t Arp = 0x0806;
	static const uint16_t Ip6 = 0x86DD;
};
typedef GEthHdr *PEthHdr;
#pragma pack(pop)
