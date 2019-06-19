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

#include "base/log/glog.h"

// ----------------------------------------------------------------------------
// GLogFile
// ----------------------------------------------------------------------------
struct GLogFile : GLog {
	void write(QString& msg) override;
};
