#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <string>
#include <ctime>

// Watches a file for modifications by polling its last-modified timestamp
class FileWatcher {
public:
    explicit FileWatcher(const std::string& filepath);

    // Check if the file has been modified since the last check
    bool hasChanged();

    // Read the entire file content
    std::string readFile() const;

private:
    std::string filepath_;
    std::time_t lastModified_;

    // Get the current modification time of the file
    std::time_t getModTime() const;
};

#endif // FILE_WATCHER_HPP
