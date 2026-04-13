#ifndef METRICS_HPP
#define METRICS_HPP

#include <string>

// Tracks performance metrics for a single buffer replacement strategy
struct Metrics {
    std::string strategyName;
    int totalRequests = 0;
    int bufferHits    = 0;
    int bufferMisses  = 0;
    int diskReads     = 0;   // Same as misses (simulated)
    int evictions     = 0;

    // Compute hit ratio (0.0 - 1.0)
    double hitRatio() const {
        if (totalRequests == 0) return 0.0;
        return static_cast<double>(bufferHits) / totalRequests;
    }

    // Compute miss ratio (0.0 - 1.0)
    double missRatio() const {
        if (totalRequests == 0) return 0.0;
        return static_cast<double>(bufferMisses) / totalRequests;
    }
};

#endif // METRICS_HPP
