#include "gdot11packet.h"

// ----------------------------------------------------------------------------
// GDot11Packet
// ----------------------------------------------------------------------------
void GDot11Packet::parse() {
	qDebug() << "not supported yet"; // gilgil temp 2019.05.25
}

GPacket* GDot11Packet::clone() {
	GPacket* res = new GDot11Packet;
	res->doClone(this);
	return res;
}
