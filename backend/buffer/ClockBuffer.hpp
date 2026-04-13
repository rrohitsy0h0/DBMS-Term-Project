#ifndef CLOCK_BUFFER_HPP
#define CLOCK_BUFFER_HPP

#include "BufferManager.hpp"
#include "../models/Page.hpp"
#include "../models/Metrics.hpp"
#include <vector>
#include <unordered_map>

// CLOCK (Second-Chance) buffer replacement strategy
class ClockBuffer : public BufferManager {
public:
    explicit ClockBuffer(int capacity);

    void accessPage(int pageId) override;
    Metrics getMetrics() const override;
    std::string getName() const override;

private:
    int capacity_;
    Metrics metrics_;

    // Circular buffer of pages
    std::vector<Page> frames_;

    // Maps page ID → index in frames_ for O(1) lookup
    std::unordered_map<int, int> pageMap_;

    // Clock hand position
    int hand_;

    // Current number of occupied frames
    int count_;

    // Find a victim frame using the clock algorithm
    int findVictim();
};

#endif // CLOCK_BUFFER_HPP
