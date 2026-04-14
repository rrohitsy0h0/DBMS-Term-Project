#include "LRUBuffer.hpp"

LRUBuffer::LRUBuffer(int capacity) : capacity_(capacity) {
  metrics_.strategyName = "LRU";
}

void LRUBuffer::accessPage(int pageId) {
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
      int evictId = useOrder_.back();
      useOrder_.pop_back();
      pageMap_.erase(evictId);
      metrics_.evictions++;
    }

    useOrder_.push_front(pageId);
    pageMap_[pageId] = useOrder_.begin();
  }
}

Metrics LRUBuffer::getMetrics() const { return metrics_; }

std::string LRUBuffer::getName() const { return "LRU"; }
