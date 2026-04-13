#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

namespace Config {
    // Buffer pool size (number of frames)
    constexpr int BUFFER_SIZE = 10;

    // Number of records per page
    constexpr int PAGE_SIZE = 5;

    // Polling interval in milliseconds
    constexpr int POLL_INTERVAL_MS = 500;

    // File paths
    const std::string DB_PATH       = "database.db";
    const std::string QUERY_FILE    = "input/query.sql";
    const std::string METRICS_FILE  = "output/metrics.txt";
    const std::string DISPLAY_FILE  = "output/display.txt";
}

#endif // CONFIG_HPP
