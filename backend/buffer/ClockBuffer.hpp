#ifndef CLOCK_BUFFER_HPP
#define CLOCK_BUFFER_HPP

#include "../models/Metrics.hpp"
#include "../models/Page.hpp"
#include "BufferManager.hpp"
#include <unordered_map>
#include <vector>

class ClockBuffer : public BufferManager {
public:
  explicit ClockBuffer(int capacity);

  void accessPage(int pageId) override;
  Metrics getMetrics() const override;
  std::string getName() const override;

private:
  int capacity_;
  Metrics metrics_;

  std::vector<Page> frames_;

  std::unordered_map<int, int> pageMap_;

  int hand_;

  int count_;

  int findVictim();
};

#endif
