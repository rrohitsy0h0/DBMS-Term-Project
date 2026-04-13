#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

namespace Logger {

    // Get current timestamp as formatted string
    inline std::string getTimestamp() {
        std::time_t now = std::time(nullptr);
        std::tm* lt = std::localtime(&now);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", lt);
        return std::string(buf);
    }

    inline void info(const std::string& msg) {
        std::cout << "[INFO]  [" << getTimestamp() << "] " << msg << std::endl;
    }

    inline void warn(const std::string& msg) {
        std::cout << "[WARN]  [" << getTimestamp() << "] " << msg << std::endl;
    }

    inline void error(const std::string& msg) {
        std::cerr << "[ERROR] [" << getTimestamp() << "] " << msg << std::endl;
    }

} // namespace Logger

#endif // LOGGER_HPP
