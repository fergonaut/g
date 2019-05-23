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

#include "base/gstateobj.h"
#include "net/packet/gpacket.h"
#include "base/gwaitevent.h"

// ----------------------------------------------------------------------------
// GSleep
// ----------------------------------------------------------------------------
struct G_EXPORT GSleep : GStateObj {
  Q_OBJECT
  Q_PROPERTY(ulong duration MEMBER duration_) // msec

public:
  ulong duration_{1000};

protected:
  GWaitEvent we_;

public:
  Q_INVOKABLE GSleep(QObject* parent = nullptr) : GStateObj(parent) {}
  ~GSleep() override {}

protected:
  bool doOpen() override;
  bool doClose() override;

public slots:
  void sleep(GPacket* packet);

signals:
  void sleeped(GPacket* packet);
};
