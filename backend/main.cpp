

#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "buffer/BufferManager.hpp"
#include "buffer/ClockBuffer.hpp"
#include "buffer/LRUBuffer.hpp"
#include "buffer/MRUBuffer.hpp"
#include "buffer/PinnedBuffer.hpp"
#include "config/Config.hpp"
#include "database/SQLiteManager.hpp"
#include "utils/Helpers.hpp"
#include "utils/Logger.hpp"
#include "watcher/FileWatcher.hpp"

static std::vector<BufferManager *> g_managers;
static volatile sig_atomic_t g_running = 1;

void sigintHandler(int /*sig*/) { g_running = 0; }

int main() {

  std::signal(SIGINT, sigintHandler);
  std::signal(SIGTERM, sigintHandler);

  Logger::info("=== DBMS Buffer Manager Simulation ===");
  Logger::info("Buffer size: " + std::to_string(Config::BUFFER_SIZE) +
               " frames");
  Logger::info("Page size: " + std::to_string(Config::PAGE_SIZE) +
               " records/page");
  Logger::info("Polling interval: " + std::to_string(Config::POLL_INTERVAL_MS) +
               " ms");
  Logger::info("Press Ctrl+C to stop and dump metrics.");

  SQLiteManager dbManager(Config::DB_PATH);
  if (!dbManager.isOpen()) {
    Logger::error("Failed to open database. Exiting.");
    return 1;
  }

  Logger::info("Initializing page offset registry...");
  dbManager.initPageOffsets(Config::PAGE_SIZE);

  FileWatcher watcher(Config::QUERY_FILE);

  LRUBuffer lruBuffer(Config::BUFFER_SIZE);
  MRUBuffer mruBuffer(Config::BUFFER_SIZE);
  ClockBuffer clockBuffer(Config::BUFFER_SIZE);
  PinnedBuffer pinnedBuffer(Config::BUFFER_SIZE);

  g_managers = {&lruBuffer, &mruBuffer, &clockBuffer, &pinnedBuffer};

  Logger::info("All buffer managers initialized. Watching for queries...\n");

  {
    std::ofstream(Config::DISPLAY_FILE, std::ios::trunc);
  }
  {
    std::ofstream(Config::QUERY_FILE, std::ios::trunc);
  }

  while (g_running) {
    if (watcher.hasChanged()) {

      std::string sql = watcher.readFile();

      while (!sql.empty() &&
             (sql.back() == '\n' || sql.back() == '\r' || sql.back() == ' ')) {
        sql.pop_back();
      }

      if (sql.empty()) {
        Logger::warn("query.sql is empty. Skipping.");
        continue;
      }

      Logger::info("New query detected: " + sql);

      QueryResult result = dbManager.executeQueryWithResults(sql);
      int recordCount = result.recordCount();

      if (recordCount <= 0) {
        Logger::warn("Query returned 0 records. Skipping buffer simulation.");
        continue;
      }

      Helpers::appendQueryResultToFile(sql, result, Config::DISPLAY_FILE, true);
      Logger::info("Query results written to " + Config::DISPLAY_FILE);

      std::string tableName = SQLiteManager::extractTableName(sql);
      int basePageId = dbManager.getBasePageId(tableName);

      std::vector<int> pageSequence = Helpers::generatePageSequence(
          recordCount, Config::PAGE_SIZE, basePageId);

      Logger::info("Table: '" + tableName +
                   "' | Base page: " + std::to_string(basePageId) +
                   " | Pages: [" + std::to_string(pageSequence.front()) + ".." +
                   std::to_string(pageSequence.back()) + "]");

      for (BufferManager *mgr : g_managers) {
        for (int pageId : pageSequence) {
          mgr->accessPage(pageId);
        }
      }

      Logger::info("--- Quick Metrics Summary ---");
      for (BufferManager *mgr : g_managers) {
        Metrics m = mgr->getMetrics();
        Logger::info(m.strategyName +
                     " | Hits: " + std::to_string(m.bufferHits) +
                     " | Misses: " + std::to_string(m.bufferMisses) +
                     " | Evictions: " + std::to_string(m.evictions));
      }
      Logger::info("-----------------------------\n");
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(Config::POLL_INTERVAL_MS));
  }

  Logger::info("\nSIGINT received. Dumping metrics to " + Config::METRICS_FILE +
               "...");

  Helpers::dumpMetricsToFile(g_managers, Config::METRICS_FILE);

  Logger::info("Metrics written to " + Config::METRICS_FILE);
  Logger::info("Goodbye!");

  return 0;
}
