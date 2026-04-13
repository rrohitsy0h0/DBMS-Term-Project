#ifndef MRU_BUFFER_HPP
#define MRU_BUFFER_HPP

#include "BufferManager.hpp"
#include "../models/Metrics.hpp"
#include <list>
#include <unordered_map>

// Most Recently Used buffer replacement strategy
class MRUBuffer : public BufferManager {
public:
    explicit MRUBuffer(int capacity);

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
};

#endif // MRU_BUFFER_HPP
