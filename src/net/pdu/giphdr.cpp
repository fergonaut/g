#include "giphdr.h"
#include "gethhdr.h"
#include "net/packet/gpacket.h"

// ----------------------------------------------------------------------------
// GIpHdr
// ----------------------------------------------------------------------------
size_t GIpHdr::size() {
  Q_ASSERT(ip_hdr_ != nullptr);
  return ip_hdr_->ip_hl * 4;
}

GIpHdr::GIpHdr(u_char* buf) {
  ip_hdr_ = (IP_HDR*)buf;
}

// ----------------------------------------------------------------------------
// GIpParser
// ----------------------------------------------------------------------------
bool GIpParser::isMatch(GPdu* prev, GPacket* packet) {
  Q_ASSERT(dynamic_cast<GEthHdr*>(prev) != nullptr);
  GEthHdr* ethHdr = (GEthHdr*)prev;
  if (ethHdr->type() != ETHERTYPE_IP)
    return false;
  if (packet->parse_.size_ < sizeof(IP_HDR))
    return false;
  return true;
}

GPdu* GIpParser::doParse(GPacket* packet) {
  Q_ASSERT(packet->parse_.size_ >= sizeof(IP_HDR));
#ifdef _DEBUG
  if (*packet->parse_.data_ != 0x45) {
    qWarning() << "packet start is not 0x45" << *packet->parse_.data_;
  }
#endif // _DEBUG
  return new GIpHdr(packet->parse_.data_);
}
