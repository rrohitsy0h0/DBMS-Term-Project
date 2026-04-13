#include "ClockBuffer.hpp"

ClockBuffer::ClockBuffer(int capacity)
    : capacity_(capacity), hand_(0), count_(0)
{
    metrics_.strategyName = "CLOCK";
    frames_.resize(capacity_);
}

int ClockBuffer::findVictim() {
    // Sweep the clock hand looking for a page with referenceBit == false
    while (true) {
        if (!frames_[hand_].referenceBit) {
            int victim = hand_;
            hand_ = (hand_ + 1) % capacity_;
            return victim;
        }
        // Give second chance: clear reference bit and advance
        frames_[hand_].referenceBit = false;
        hand_ = (hand_ + 1) % capacity_;
    }
}

void ClockBuffer::accessPage(int pageId) {
    metrics_.totalRequests++;

    auto it = pageMap_.find(pageId);

    if (it != pageMap_.end()) {
        // HIT: page already in buffer — set reference bit
        metrics_.bufferHits++;
        frames_[it->second].referenceBit = true;
    } else {
        // MISS: page not in buffer
        metrics_.bufferMisses++;
        metrics_.diskReads++;

        int frameIdx;

        if (count_ < capacity_) {
            // Buffer not full yet — use next empty frame
            frameIdx = count_;
            count_++;
        } else {
            // Buffer full — find victim using clock algorithm
            frameIdx = findVictim();

            // Remove old page from map
            int oldPageId = frames_[frameIdx].id;
            pageMap_.erase(oldPageId);
            metrics_.evictions++;
        }

        // Place new page in frame
        frames_[frameIdx] = Page(pageId);
        pageMap_[pageId] = frameIdx;
    }
}

Metrics ClockBuffer::getMetrics() const {
    return metrics_;
}

std::string ClockBuffer::getName() const {
    return "CLOCK";
}
