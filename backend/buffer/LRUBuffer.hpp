#ifndef LRU_BUFFER_HPP
#define LRU_BUFFER_HPP

#include "../models/Metrics.hpp"
#include "BufferManager.hpp"
#include <list>
#include <unordered_map>

class LRUBuffer : public BufferManager {
public:
  explicit LRUBuffer(int capacity);

  void accessPage(int pageId) override;
  Metrics getMetrics() const override;
  std::string getName() const override;

private:
  int capacity_;
  Metrics metrics_;

  std::list<int> useOrder_;

  std::unordered_map<int, std::list<int>::iterator> pageMap_;
};

#endif
