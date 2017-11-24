#include "gpdu.h"

// ----------------------------------------------------------------------------
// GPdus
// ----------------------------------------------------------------------------
void GPdus::clear() {
   current_ = 0;

  int count = size();
  for (int i = count - 1; i >= 0; i--) {
    GPdu* pdu = this->at(i);
    delete pdu;
  }
  QVector<GPdu*>::clear();
}

void GPdus::add(GPdu* pdu) {
  push_back(pdu);
}
