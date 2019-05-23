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

#include "base/gbase.h"

// ----------------------------------------------------------------------------
// GBuf
// ----------------------------------------------------------------------------
struct G_EXPORT GBuf final {
  gbyte* data_;
  size_t size_;

  void clear() {
    data_ = nullptr;
    size_ = 0;
  }

  bool valid() {
    return data_ != nullptr;
  }
};
