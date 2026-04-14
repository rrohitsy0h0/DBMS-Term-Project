#include "FileWatcher.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

FileWatcher::FileWatcher(const std::string &filepath)
    : filepath_(filepath), lastModified_(0) {}

std::time_t FileWatcher::getModTime() const {
  struct stat fileStat;
  if (stat(filepath_.c_str(), &fileStat) != 0) {
    return 0;
  }
  return fileStat.st_mtime;
}

bool FileWatcher::hasChanged() {
  std::time_t currentMod = getModTime();
  if (currentMod != lastModified_ && currentMod != 0) {
    lastModified_ = currentMod;
    return true;
  }
  return false;
}

std::string FileWatcher::readFile() const {
  std::ifstream ifs(filepath_);
  if (!ifs.is_open()) {
    return "";
  }
  std::stringstream ss;
  ss << ifs.rdbuf();
  return ss.str();
}
