#ifndef MRU_BUFFER_HPP
#define MRU_BUFFER_HPP

#include "../models/Metrics.hpp"
#include "BufferManager.hpp"
#include <list>
#include <unordered_map>

class MRUBuffer : public BufferManager {
public:
  explicit MRUBuffer(int capacity);

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
