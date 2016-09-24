#include "gpcapdevicewriter.h"

// ----------------------------------------------------------------------------
// GPcapDeviceWriter
// ----------------------------------------------------------------------------
GPcapDeviceWriter::GPcapDeviceWriter(QObject* parent) : GProcess(parent) {

}

GPcapDeviceWriter::~GPcapDeviceWriter() {
  close();
}

bool GPcapDeviceWriter::doOpen() {
  if (dev_ == "") {
    SET_ERR(GErr::DEVICE_NOT_SPECIFIED, "device is not specified");
    return false;
  }

  char errBuf[PCAP_ERRBUF_SIZE];
  pcap_ = pcap_open_live(qPrintable(dev_), 0, 0, 0, errBuf);
  if (pcap_ == nullptr) {
    SET_ERR(GErr::RETURN_NULL, errBuf);
    return false;
  }

  return true;
}

bool GPcapDeviceWriter::doClose()  {
  if (pcap_ != nullptr) {
    pcap_close(pcap_);
    pcap_ = nullptr;
  }
  return true;
}

GPacket::Result GPcapDeviceWriter::write(GPacket* packet) {
  int i = pcap_sendpacket(pcap_, packet->buf_, packet->pkthdr_.caplen);
  if (i == 0) return GPacket::Ok;
  qWarning() << QString("pcap_sendpacket return %1").arg(i);
  return GPacket::Fail;
}
