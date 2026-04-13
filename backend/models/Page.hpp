#ifndef PAGE_HPP
#define PAGE_HPP

// Represents a single page in the buffer pool
struct Page {
    int id;                  // Page identifier
    bool pinned;             // Whether this page is pinned (cannot be evicted)
    bool referenceBit;       // Used by CLOCK replacement algorithm

    Page() : id(-1), pinned(false), referenceBit(false) {}

    Page(int id, bool pinned = false)
        : id(id), pinned(pinned), referenceBit(true) {}
};

#endif // PAGE_HPP
