#ifndef BUFFER_MANAGER_HPP
#define BUFFER_MANAGER_HPP

#include "../models/Metrics.hpp"
#include <string>

class BufferManager {
public:
  virtual ~BufferManager() = default;

  virtual void accessPage(int pageId) = 0;

  virtual Metrics getMetrics() const = 0;

  virtual std::string getName() const = 0;
};

#endif
