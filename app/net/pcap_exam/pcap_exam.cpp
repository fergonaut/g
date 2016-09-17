#include <iostream>
#include <QCoreApplication>
#include <GApp>
#include <GEthHdr>
#include <GIpHdr>
#include <GJson>
#include <GPcapDevice>
#include <GTcpHdr>

struct Obj : QObject {
  Q_OBJECT

public slots:
  void captured(GPacket* packet) {
    GEthHdr* ethHdr = packet->findFirst<GEthHdr>();
    if (ethHdr == nullptr) return;

    GIpHdr* ipHdr = packet->findNext<GIpHdr>();
    if (ipHdr == nullptr) return;

    GTcpHdr* tcpHdr = packet->findNext<GTcpHdr>();
    if (tcpHdr == nullptr) return;

    QString smac  = (QString)ethHdr->smac();
    QString dmac  = (QString)ethHdr->dmac();
    QString sip   = (QString)ipHdr->sip();
    QString dip   = (QString)ipHdr->dip();
    QString sport = QString::number(tcpHdr->sport());
    QString dport = QString::number(tcpHdr->dport());

    static int count = 0;
    QString msg = QString("%1 %2 > %3 %4:%5 > %6:%7\n").
      arg(++count).arg(smac, dmac, sip, sport, dip, dport);
    std::clog << qPrintable(msg);
  }
};

int main(int argc, char* argv[]) {
  QCoreApplication a(argc, argv);
  GApp::init();

  GPcapDevice device;

  QJsonObject jo = GJson::loadFromFile();
  jo["device"] >> device;
  jo["device"] << device;
  GJson::saveToFile(jo);

  Obj obj;
  QObject::connect(&device, &GCapture::captured, &obj, &Obj::captured, Qt::DirectConnection);

  if (!device.open()) {
    std::clog << device.err;
    return EXIT_FAILURE;
  }

  std::clog << "Press enter key to close\n";
  while (std::cin.get() != '\n');

  return EXIT_SUCCESS;
}

#include "pcap_exam.moc"
