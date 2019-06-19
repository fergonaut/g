#include <iostream>
#include <GApp>
#include <GNetIntf>
#include <GRtm>

using namespace std;

void showNetIntf() {
	GRtmEntry* entry = GRtm::instance().getBestEntry("8.8.8.8");
	GNetIntfs& intfs = GNetIntf::all();

	int index = 1;
	for(GNetIntf& intf: intfs) {
		bool best = entry != nullptr && entry->intf()->name() == intf.name();
		QString msg = QString("index %1 %2").arg(index).arg(best ? "(Best)" : "");
		clog << qPrintable(msg) << endl;

		msg = QString("  %1").arg(intf.name());
		if (intf.desc() != "") msg += QString(" / %1").arg(intf.desc());
		clog << qPrintable(msg) << endl;

		msg = "";
		if (!intf.mac().isClean()) msg += QString("mac:%1 ").arg(QString(intf.mac()));
		if (intf.ip() != 0) msg += QString("ip:%1 ").arg(QString(intf.ip()));
		if (intf.mask() != 0) msg += QString("mask:%1 ").arg(QString(intf.mask()));
		if (intf.gateway() != 0) msg += QString("gateway:%1 ").arg(QString(intf.gateway()));
		if (msg != "") {
			msg = QString("  %1").arg(msg);
			clog << qPrintable(msg) << endl;
		}

		clog << endl;
		index++;
	}
}

void showRtm() {
	GRtm& rtm = GRtm::instance();

	clog << "dst             mask            gateway         metric intf" << endl;
	for (GRtmEntry& entry: rtm) {
		QString dstStr = QString(entry.dst()); dstStr += QString(" ").repeated(16 - dstStr.length());
		QString maskStr = QString(entry.mask()); maskStr += QString(" ").repeated(16 - maskStr.length());
		QString gatewayStr = QString(entry.gateway()); gatewayStr += QString(" ").repeated(16 - gatewayStr.length());
		QString metricStr = QString::number(entry.metric()); metricStr += QString(" ").repeated(7 - metricStr.length());
		QString intfStr = QString("%1 %2").arg(entry.intf()->index()).arg(entry.intf()->desc());
		QString msg = dstStr + maskStr + gatewayStr + metricStr + intfStr;
		clog << qPrintable(msg) << endl;
	}
}

int main() {
	GApp::init();
	showNetIntf();
	showRtm();
}
