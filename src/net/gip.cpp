#include "base/gerr.h"
#include "gip.h"
#include <cstdlib> // for atoi

// ----------------------------------------------------------------------------
// GIp
// ----------------------------------------------------------------------------
GIp::GIp(const QString& rhs) {
	std::string s = rhs.toStdString();
	const char* p = s.c_str();

#ifdef Q_OS_WIN
	LONG res = RtlIpv4StringToAddressA(p, true, nullptr, reinterpret_cast<IN_ADDR*>(&ip_)) ;
	if (res != 0/*STATUS_SUCCESS*/) {
		qWarning() << "RtlIpv4StringToAddressA return" << res;
		ip_ = 0;
	}
#endif


	int res = inet_pton(AF_INET6, p, &ip6_);
	switch (res) {
	case 0:
		qWarning() << "inet_pton return zero ip=" << rhs;
		break;
	case 1: // succeed
		break;
	default:
		qWarning() << "inet_pton return " << res << " " << GLastErr();
	}
}

GIp::operator QString() const {
	char s[INET_ADDRSTRLEN];
	sprintf(s, "%d.%d.%d.%d",
	(ip_ & 0xFF000000) >> 24,
	(ip_ & 0x00FF0000) >> 16,
	(ip_ & 0x0000FF00) >> 8,
	(ip_ & 0x000000FF) >> 0);
	return QString(s);
}

// ----------------------------------------------------------------------------
// GTEST
// ----------------------------------------------------------------------------
#ifdef GTEST
#include <gtest/gtest.h>

TEST(GIp, ctorTest) {
	GIp ip1; // ()

	GIp ip2{ip1}; // (const GIp& rhs)

	GIp ip3{0x7F000001}; // (const uint32_t rhs)
	EXPECT_EQ(ip3, 0x7F000001);

	GIp ip4{QString("127.0.0.1")}; // (const QString& rhs)
	EXPECT_EQ(ip4, 0x7F000001);

	GIp ip5{"127.0.0.1"}; // (const QString& rhs)
	EXPECT_EQ(ip5, 0x7F000001);
}

TEST(GIp, castingTest) {
	GIp ip{"127.0.0.1"};

	uint32_t ui; ui = ip; // operator uint32_t()
	EXPECT_EQ(ui, 0x7F000001);

	QString s2; s2 = QString(ip); // operator QString()
	EXPECT_EQ(s2, "127.0.0.1");
}

TEST(GIp, funcTest) {
	GIp ip;

	ip.clear();
	EXPECT_EQ(ip, 0);

	ip = QString("127.0.0.1");
	EXPECT_TRUE(ip.isLocalHost());

	ip = QString("255.255.255.255");
	EXPECT_TRUE(ip.isBroadcast());

	ip = QString("224.0.0.0");
	EXPECT_TRUE(ip.isMulticast());
}

#endif // GTEST
