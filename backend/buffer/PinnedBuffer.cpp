#include "PinnedBuffer.hpp"

PinnedBuffer::PinnedBuffer(int capacity) : capacity_(capacity) {
    metrics_.strategyName = "PINNED";
}

bool PinnedBuffer::shouldPin(int pageId) const {
    // Simulation heuristic: pin every 7th page (pages 0, 7, 14, 21, ...)
    return (pageId % 7 == 0);
}

void PinnedBuffer::accessPage(int pageId) {
    metrics_.totalRequests++;

    auto it = pageMap_.find(pageId);

    if (it != pageMap_.end()) {
        // HIT: page already in buffer — move to front
        metrics_.bufferHits++;
        useOrder_.erase(it->second);
        useOrder_.push_front(pageId);
        pageMap_[pageId] = useOrder_.begin();
    } else {
        // MISS: page not in buffer
        metrics_.bufferMisses++;
        metrics_.diskReads++;

        // If buffer is full, evict an unpinned page (LRU order from back)
        if (static_cast<int>(pageMap_.size()) >= capacity_) {
            bool evicted = false;

            // Walk from back (least recently used) to find an unpinned page
            for (auto rit = useOrder_.rbegin(); rit != useOrder_.rend(); ++rit) {
                if (pinnedPages_.find(*rit) == pinnedPages_.end()) {
                    // Found an unpinned page — evict it
                    int evictId = *rit;
                    pageMap_.erase(evictId);

                    // Convert reverse iterator to forward iterator for erase
                    auto fwd = std::next(rit).base();
                    useOrder_.erase(fwd);

                    metrics_.evictions++;
                    evicted = true;
                    break;
                }
            }

            // If all pages are pinned, we cannot evict — skip this access
            if (!evicted) {
                return;
            }
        }

        // Insert new page at front
        useOrder_.push_front(pageId);
        pageMap_[pageId] = useOrder_.begin();

        // Pin the page if it qualifies
        if (shouldPin(pageId)) {
            pinnedPages_.insert(pageId);
        }
    }
}

Metrics PinnedBuffer::getMetrics() const {
    return metrics_;
}

std::string PinnedBuffer::getName() const {
    return "PINNED";
}
