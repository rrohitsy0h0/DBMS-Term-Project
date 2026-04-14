#ifndef PAGE_HPP
#define PAGE_HPP

struct Page {
  int id;
  bool pinned;
  bool referenceBit;

  Page() : id(-1), pinned(false), referenceBit(false) {}

  Page(int id, bool pinned = false)
      : id(id), pinned(pinned), referenceBit(true) {}
};

#endif
