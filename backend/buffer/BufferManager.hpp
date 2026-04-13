#ifndef BUFFER_MANAGER_HPP
#define BUFFER_MANAGER_HPP

#include "../models/Metrics.hpp"
#include <string>

// Abstract base class for all buffer replacement strategies
class BufferManager {
public:
    virtual ~BufferManager() = default;

    // Access a page by its ID. Handles hit/miss logic internally.
    virtual void accessPage(int pageId) = 0;

    // Return current accumulated metrics
    virtual Metrics getMetrics() const = 0;

    // Return the strategy name
    virtual std::string getName() const = 0;
};

#endif // BUFFER_MANAGER_HPP
