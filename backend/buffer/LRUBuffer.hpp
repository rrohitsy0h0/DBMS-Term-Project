#ifndef LRU_BUFFER_HPP
#define LRU_BUFFER_HPP

#include "BufferManager.hpp"
#include "../models/Metrics.hpp"
#include <list>
#include <unordered_map>

// Least Recently Used buffer replacement strategy
class LRUBuffer : public BufferManager {
public:
    explicit LRUBuffer(int capacity);

    void accessPage(int pageId) override;
    Metrics getMetrics() const override;
    std::string getName() const override;

private:
    int capacity_;
    Metrics metrics_;

    // Doubly-linked list: front = most recently used, back = least recently used
    std::list<int> useOrder_;

    // Maps page ID → iterator in useOrder_ for O(1) lookup and move
    std::unordered_map<int, std::list<int>::iterator> pageMap_;
};

#endif // LRU_BUFFER_HPP
