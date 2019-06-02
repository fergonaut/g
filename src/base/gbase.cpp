#include "gbase.h"
#include <pcap.h>

const GDuration G::Timeout = 5000;

const char* G::version() {
#ifdef _DEBUG
  return "G Library version 0.2 Debug Build(" __DATE__ " " __TIME__ ")";
#else // RELEASE
  return "G Library version 0.2 Release Build(" __DATE__ " " __TIME__ ")";
#endif // _DEBUG
}

const char* G::pcap_lib_version() {
  return ::pcap_lib_version();
}

// ----------------------------------------------------------------------------
// GTEST
// ----------------------------------------------------------------------------
#ifdef GTEST
#include <gtest/gtest.h>

#include <iostream>
TEST(GBase, versionTest) {
  std::clog << G::Version << std::endl;
  std::clog << G::pcap_lib_version() << std::endl;
}

#endif // GTEST
