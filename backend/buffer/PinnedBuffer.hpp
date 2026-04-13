#ifndef PINNED_BUFFER_HPP
#define PINNED_BUFFER_HPP

#include "BufferManager.hpp"
#include "../models/Metrics.hpp"
#include <list>
#include <unordered_map>
#include <unordered_set>

// Pinned-page buffer replacement strategy (LRU-based, but pinned pages cannot be evicted)
class PinnedBuffer : public BufferManager {
public:
    explicit PinnedBuffer(int capacity);

    void accessPage(int pageId) override;
    Metrics getMetrics() const override;
    std::string getName() const override;

private:
    int capacity_;
    Metrics metrics_;

    // Doubly-linked list: front = most recently used
    std::list<int> useOrder_;

    // Maps page ID → iterator in useOrder_
    std::unordered_map<int, std::list<int>::iterator> pageMap_;

    // Set of currently pinned page IDs in the buffer
    std::unordered_set<int> pinnedPages_;

    // Determine if a page should be pinned (simulation heuristic)
    bool shouldPin(int pageId) const;
};

#endif // PINNED_BUFFER_HPP
