#include "gipflowmgr.h"
#include "net/pdu/giphdr.h"

// ----------------------------------------------------------------------------
// GIpFlowMgr
// ----------------------------------------------------------------------------
void GIpFlowMgr::deleteOldFlowMaps(GPacket* packet /* struct timeval ts */) {
  struct timeval ts = packet->ts_;
  FlowMap::iterator it = flowMap_.begin();
  while (it != flowMap_.end()) {
    GFlow::Value* value = it.value();
    long elapsed = ts.tv_sec - value->ts_.tv_sec;
    long timeout;
    switch (value->state_) {
      case GFlow::Value::Half: timeout = halfTimeout_; break;
      case GFlow::Value::Full: timeout = fullTimeout_; break;
      case GFlow::Value::Rst: qCritical() << "unrecheable Rst"; timeout = 0; break;
      case GFlow::Value::Fin: qCritical() << "unrecheable Fin"; timeout = 0; break;
    }
    if (elapsed >= timeout) {
      key_ = (GFlow::IpFlowKey*)&it.key();
      value_ = value;
      emit _flowDeleted(packet);
      it = flowMap_.erase(it);
      continue;
    }
    it++;
  }
}

void GIpFlowMgr::process(GPacket* packet) {
  long now = packet->ts_.tv_sec;
  if (checkInterval_ != 0 && now - lastCheckTick_ >= checkInterval_)
  {
    deleteOldFlowMaps(packet);
    lastCheckTick_ = now;
  }

  GIpHdr* ipHdr = packet->pdus_.findFirst<GIpHdr>();
  if (ipHdr == nullptr) return;

  GFlow::IpFlowKey key{ipHdr->sip(), ipHdr->dip()};

  FlowMap::iterator it = flowMap_.find(key);
  if (it == flowMap_.end()) {
    GFlow::Value* value = GFlow::Value::allocate(packet->ts_, GFlow::Value::Half, requestItems_.totalMemSize_);
    it = flowMap_.insert(key, value);
    key_ = (GFlow::IpFlowKey*)&it.key();
    value_ = value;
    emit _flowCreated(packet);

    GFlow::IpFlowKey reverseKey = GFlow::IpFlowKey(it.key()).reverse();
    FlowMap::iterator reverseIt = flowMap_.find(reverseKey);
    if (reverseIt != flowMap_.end()) {
      it.value()->state_ = GFlow::Value::Full;
      reverseIt.value()->state_ = GFlow::Value::Full;
    }
  } else {
    GFlow::Value* value = it.value();
    value->ts_ = packet->ts_;
  }

  this->key_ = (GFlow::IpFlowKey*)&it.key();
  this->value_ = it.value();
  emit processed(packet);
}
