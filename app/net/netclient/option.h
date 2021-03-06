#pragma once

#include <QSsl>
#include <GObj>

// ----------------------------------------------------------------------------
// Option
// ----------------------------------------------------------------------------
struct NetClient : GObj {
	Q_OBJECT
	Q_PROPERTY(QString host MEMBER host_)
	Q_PROPERTY(quint16 port MEMBER port_)
	Q_PROPERTY(QString localHost MEMBER localHost_)
	Q_PROPERTY(quint16 localPort MEMBER localPort_)

public:
	QString host_{""};
	quint16 port_{0};
	QString localHost_{""};
	quint16 localPort_{0};

public:
};

struct TcpClient : NetClient {
};

struct UdpClient : NetClient {
};

struct SslClient : NetClient {
	Q_OBJECT
	//Q_PROPERTY(SslProtocol protocol READ protocol() WRITE setProtocol)
	Q_PROPERTY(SslProtocol protocol MEMBER protocol_)
	Q_ENUMS(SslProtocol)

public:
	enum SslProtocol {
		SslV3,
		SslV2,
		TlsV1_0,
#if QT_DEPRECATED_SINCE(5,0)
		TlsV1 = TlsV1_0,
#endif
		TlsV1_1,
		TlsV1_2,
		AnyProtocol,
		TlsV1SslV3,
		SecureProtocols,

		TlsV1_0OrLater,
		TlsV1_1OrLater,
		TlsV1_2OrLater,

		DtlsV1_0,
		DtlsV1_0OrLater,
		DtlsV1_2,
		DtlsV1_2OrLater,

		TlsV1_3,
		TlsV1_3OrLater,

		UnknownProtocol = -1
	};

	//int protocol() { return (int) protocol_; }
	//void setProtocol(int protocol) { protocol_ = (QSsl::SslProtocol)protocol; }

public:
	SslProtocol protocol_;
};

struct Option : GObj {
	Q_OBJECT
	Q_PROPERTY(GObjRef tcpClient READ getTcpClient)
	Q_PROPERTY(GObjRef udpClient READ getUdpClient)
	Q_PROPERTY(GObjRef sslClient READ getSslClient)

	GObjRef getTcpClient() { return &tcpClient_; }
	GObjRef getUdpClient() { return &udpClient_; }
	GObjRef getSslClient() { return &sslClient_; }

public:
	TcpClient tcpClient_;
	UdpClient udpClient_;
	SslClient sslClient_;
};
