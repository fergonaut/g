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

#include "net/capture/gpcapdevice.h"
#include "net/packet/gpacket.h"
#include "net/gnetintf.h"
#include "net/pdu/gethhdr.h"
#include "net/pdu/garphdr.h"
#include "base/gwaitevent.h"

// ----------------------------------------------------------------------------
// GEthArpHdr
// ----------------------------------------------------------------------------
#pragma pack(push, 1)
struct GEthArpHdr {
  GEthHdr ethHdr_;
  GArpHdr arpHdr_;
};
#pragma pack(pop)

// ----------------------------------------------------------------------------
// GAtm(Arp Table Manager)
// ----------------------------------------------------------------------------
typedef QMap<GIp, GMac> GAtmMap;
struct G_EXPORT GAtm : GAtmMap {
  bool allResolved();
  bool waitAll(GPcapDevice* device, unsigned long timeout = G::Timeout);
  static GMac waitOne(GIp ip, GPcapDevice* device, unsigned long timeout = G::Timeout);

protected:
  bool sendQueries(GPcapDevice* device, GNetIntf* intf);

protected:
  // --------------------------------------------------------------------------
  // SendThread
  // --------------------------------------------------------------------------
  struct SendThread : QThread {
    SendThread(GAtm* resolve, GPcapDevice* device, GNetIntf* intf, unsigned long timeout);
    GAtm* atm_;
    GPcapDevice* device_;
    GNetIntf* intf_;
    unsigned long timeout_;
    GWaitEvent we_;
  protected:
    void run() override;
  };
};

