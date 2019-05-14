#include <QDebug>
#include "gnetintf.h"
#include "grtm.h"

// ----------------------------------------------------------------------------
// GNetInft
// ----------------------------------------------------------------------------
GNetIntfs& GNetIntf::all() {
  return GNetIntfs::instance();
}

// ----------------------------------------------------------------------------
// GNetIntfs
// ----------------------------------------------------------------------------
#include <unistd.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

static GMac getMac(char* name) {
  int s;
  struct ifreq buffer;

  s = socket(PF_INET, SOCK_DGRAM, 0);
  memset(&buffer, 0x00, sizeof(buffer));

  strcpy(buffer.ifr_name, name);
  ioctl(s, SIOCGIFHWADDR, &buffer);
  close(s);

  const u_char* p = const_cast<const u_char*>(reinterpret_cast<u_char*>(buffer.ifr_ifru.ifru_hwaddr.sa_data));
  GMac res = p;
  return res;
}

GNetIntfs::GNetIntfs() {
  //
  // Initialize allDevs using pcap API.
  //
  if (allDevs_ != nullptr) return;

  char errBuf[PCAP_ERRBUF_SIZE];

  int i = pcap_findalldevs(&allDevs_, errBuf);
  if (i != 0) { // if error occured
    qWarning() << QString("error in pcap_findalldevs_ex (%s)").arg(errBuf);
    return;
  }

  //
  // Add all interfaces
  //
  pcap_if_t* dev = allDevs_;
  i = 1;
  while (dev != nullptr) {
    GNetIntf intf;

    intf.index_ = i;
    intf.name_ = dev->name;
    intf.description_ = dev->description;
    intf.dev_ = dev;

    for(pcap_addr_t* pa = dev->addresses; pa != nullptr; pa = pa->next) {
      // mac_
      intf.mac_ = getMac(dev->name);

      // ip_
      sockaddr* addr = pa->addr;
      sockaddr_in* addr_in = reinterpret_cast<sockaddr_in*>(addr);
      if(addr != nullptr && addr->sa_family == AF_INET)
        intf.ip_ = ntohl(addr_in->sin_addr.s_addr);

      // mask_;
      addr = pa->netmask;
      addr_in = reinterpret_cast<sockaddr_in*>(addr);
      if(addr != nullptr && addr->sa_family == AF_INET) {
        intf.mask_ = ntohl(addr_in->sin_addr.s_addr);
      }

      // gateway_
      intf.gateway_ = GRtm::instance().getGateway(intf.name_);
    }

    push_back(intf);
    dev = dev->next;
    i++;
  }
}

GNetIntfs::~GNetIntfs() {
  clear();

  //
  // Finalize allDevs_
  //
  if (allDevs_ != nullptr)
  {
    pcap_freealldevs(allDevs_);
    allDevs_ = nullptr;
  }
}

GNetIntfs& GNetIntfs::instance() {
  static GNetIntfs intfs;
  return intfs;
}
