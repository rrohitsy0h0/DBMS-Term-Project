#ifndef METRICS_HPP
#define METRICS_HPP

#include <string>

struct Metrics {
  std::string strategyName;
  int totalRequests = 0;
  int bufferHits = 0;
  int bufferMisses = 0;
  int diskReads = 0;
  int evictions = 0;

  double hitRatio() const {
    if (totalRequests == 0)
      return 0.0;
    return static_cast<double>(bufferHits) / totalRequests;
  }

  double missRatio() const {
    if (totalRequests == 0)
      return 0.0;
    return static_cast<double>(bufferMisses) / totalRequests;
  }
};

#endif
