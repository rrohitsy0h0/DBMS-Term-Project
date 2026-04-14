#include "PinnedBuffer.hpp"

PinnedBuffer::PinnedBuffer(int capacity) : capacity_(capacity) {
  metrics_.strategyName = "PINNED";
}

bool PinnedBuffer::shouldPin(int pageId) const { return (pageId % 7 == 0); }

void PinnedBuffer::accessPage(int pageId) {
  metrics_.totalRequests++;

  auto it = pageMap_.find(pageId);

  if (it != pageMap_.end()) {

    metrics_.bufferHits++;
    useOrder_.erase(it->second);
    useOrder_.push_front(pageId);
    pageMap_[pageId] = useOrder_.begin();
  } else {

    metrics_.bufferMisses++;
    metrics_.diskReads++;

    if (static_cast<int>(pageMap_.size()) >= capacity_) {
      bool evicted = false;

      for (auto rit = useOrder_.rbegin(); rit != useOrder_.rend(); ++rit) {
        if (pinnedPages_.find(*rit) == pinnedPages_.end()) {

          int evictId = *rit;
          pageMap_.erase(evictId);

          auto fwd = std::next(rit).base();
          useOrder_.erase(fwd);

          metrics_.evictions++;
          evicted = true;
          break;
        }
      }

      if (!evicted) {
        return;
      }
    }

    useOrder_.push_front(pageId);
    pageMap_[pageId] = useOrder_.begin();

    if (shouldPin(pageId)) {
      pinnedPages_.insert(pageId);
    }
  }
}

Metrics PinnedBuffer::getMetrics() const { return metrics_; }

std::string PinnedBuffer::getName() const { return "PINNED"; }
