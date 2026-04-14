#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

namespace Config {

constexpr int BUFFER_SIZE = 10;

constexpr int PAGE_SIZE = 5;

constexpr int POLL_INTERVAL_MS = 500;

const std::string DB_PATH = "database.db";
const std::string QUERY_FILE = "input/query.sql";
const std::string METRICS_FILE = "output/metrics.txt";
const std::string DISPLAY_FILE = "output/display.txt";
} // namespace Config

#endif
