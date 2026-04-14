#ifndef PINNED_BUFFER_HPP
#define PINNED_BUFFER_HPP

#include "../models/Metrics.hpp"
#include "BufferManager.hpp"
#include <list>
#include <unordered_map>
#include <unordered_set>

class PinnedBuffer : public BufferManager {
public:
  explicit PinnedBuffer(int capacity);

  void accessPage(int pageId) override;
  Metrics getMetrics() const override;
  std::string getName() const override;

private:
  int capacity_;
  Metrics metrics_;

  std::list<int> useOrder_;

  std::unordered_map<int, std::list<int>::iterator> pageMap_;

  std::unordered_set<int> pinnedPages_;

  bool shouldPin(int pageId) const;
};

#endif
