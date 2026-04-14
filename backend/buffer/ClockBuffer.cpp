#include "ClockBuffer.hpp"

ClockBuffer::ClockBuffer(int capacity)
    : capacity_(capacity), hand_(0), count_(0) {
  metrics_.strategyName = "CLOCK";
  frames_.resize(capacity_);
}

int ClockBuffer::findVictim() {

  while (true) {
    if (!frames_[hand_].referenceBit) {
      int victim = hand_;
      hand_ = (hand_ + 1) % capacity_;
      return victim;
    }

    frames_[hand_].referenceBit = false;
    hand_ = (hand_ + 1) % capacity_;
  }
}

void ClockBuffer::accessPage(int pageId) {
  metrics_.totalRequests++;

  auto it = pageMap_.find(pageId);

  if (it != pageMap_.end()) {

    metrics_.bufferHits++;
    frames_[it->second].referenceBit = true;
  } else {

    metrics_.bufferMisses++;
    metrics_.diskReads++;

    int frameIdx;

    if (count_ < capacity_) {

      frameIdx = count_;
      count_++;
    } else {

      frameIdx = findVictim();

      int oldPageId = frames_[frameIdx].id;
      pageMap_.erase(oldPageId);
      metrics_.evictions++;
    }

    frames_[frameIdx] = Page(pageId);
    pageMap_[pageId] = frameIdx;
  }
}

Metrics ClockBuffer::getMetrics() const { return metrics_; }

std::string ClockBuffer::getName() const { return "CLOCK"; }
