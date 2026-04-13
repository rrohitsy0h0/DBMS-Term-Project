#include "MRUBuffer.hpp"

MRUBuffer::MRUBuffer(int capacity) : capacity_(capacity) {
    metrics_.strategyName = "MRU";
}

void MRUBuffer::accessPage(int pageId) {
    metrics_.totalRequests++;

    auto it = pageMap_.find(pageId);

    if (it != pageMap_.end()) {
        // HIT: page already in buffer — move to front (most recently used)
        metrics_.bufferHits++;
        useOrder_.erase(it->second);
        useOrder_.push_front(pageId);
        pageMap_[pageId] = useOrder_.begin();
    } else {
        // MISS: page not in buffer
        metrics_.bufferMisses++;
        metrics_.diskReads++;

        // If buffer is full, evict the most recently used page (front of list)
        if (static_cast<int>(pageMap_.size()) >= capacity_) {
            int evictId = useOrder_.front();
            useOrder_.pop_front();
            pageMap_.erase(evictId);
            metrics_.evictions++;
        }

        // Insert new page at front
        useOrder_.push_front(pageId);
        pageMap_[pageId] = useOrder_.begin();
    }
}

Metrics MRUBuffer::getMetrics() const {
    return metrics_;
}

std::string MRUBuffer::getName() const {
    return "MRU";
}
