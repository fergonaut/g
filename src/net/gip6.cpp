#include "base/gerr.h"
#include "gip6.h"
#include <wsipv6ok.h>

// ----------------------------------------------------------------------------
// GIp6
// ----------------------------------------------------------------------------
GIp6::GIp6(const QString& rhs) {
	std::string s = rhs.toStdString();
	char* p = pchar(s.c_str());
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

GIp6::operator QString() const {
	char s[INET6_ADDRSTRLEN];
	const char* res = inet_ntop(AF_INET6, &ip6_, s, INET6_ADDRSTRLEN);
	if (res == nullptr) {
		qWarning() << "inet_ntop return null " << GLastErr();
		return QString();
	}
	return QString(s);
}

// ----------------------------------------------------------------------------
// GTEST
// ----------------------------------------------------------------------------
#ifdef GTEST
#include <gtest/gtest.h>

static u_char _loopback[GIp6::SIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};

TEST(GIp6, ctorTest) {
	GIp6 ip61; // ()

	GIp6 ip62{ip61}; // GIp6(const GIp6& rhs)

	GIp6 ip63{_loopback}; // GIp6(const u_char* rhs)
	EXPECT_EQ(ip63, _loopback);

	GIp6 ip64{QString("::1")}; // GIp6(const QString rhs)
	EXPECT_EQ(ip64, _loopback);

	GIp6 ip65{"::1"}; // GIp6(const QString rhs)
	EXPECT_EQ(ip65, _loopback);
}

TEST(GIp6, castingTest) {
	GIp6 ip6{"::1"};

	const gbyte* uc = ip6; // operator const u_char*()
	gbyte temp[GIp6::SIZE];
	for (int i = 0; i < GIp6::SIZE; i++)
		temp[i] = *uc++;
	EXPECT_EQ(ip6, temp);

	// ----- gilgil temp 2019.05.18 -----
	// Error occurrs on Windows
	/*
	QString s1 = static_cast<const char*>(ip6); // operator const char*()
	EXPECT_EQ(s1, "::1");
	*/
	// ----------------------------------

	QString s2 = QString(ip6); // operator QString()
	EXPECT_EQ(s2, "::1");
}

TEST(GIp6, funcTest) {
	GIp6 ip6;

	ip6 = QString("127.0.0.1");
	EXPECT_TRUE(ip6.isLocalHost());

	ip6 = QString("255.255.255.255");
	EXPECT_TRUE(ip6.isBroadcast());

	ip6 = QString("224.0.0.0");
	EXPECT_TRUE(ip6.isMulticast());
}

#endif // GTEST
