#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <ctime>
#include <string>

class FileWatcher {
public:
  explicit FileWatcher(const std::string &filepath);

  bool hasChanged();

  std::string readFile() const;

private:
  std::string filepath_;
  std::time_t lastModified_;

  std::time_t getModTime() const;
};

#endif
