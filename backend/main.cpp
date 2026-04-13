/*
 * DBMS Buffer Manager Simulation — Main Entry Point
 *
 * Runs a continuous loop watching for SQL queries in input/query.sql.
 * When a new query is detected, it is executed against SQLite, the
 * resulting record count is converted into a page sequence, and
 * that sequence is fed to all 4 buffer replacement strategies.
 *
 * On SIGINT (Ctrl+C), all accumulated metrics are written to
 * output/metrics.txt (always overwriting any previous content).
 */

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <csignal>

#include "config/Config.hpp"
#include "watcher/FileWatcher.hpp"
#include "database/SQLiteManager.hpp"
#include "buffer/BufferManager.hpp"
#include "buffer/LRUBuffer.hpp"
#include "buffer/MRUBuffer.hpp"
#include "buffer/ClockBuffer.hpp"
#include "buffer/PinnedBuffer.hpp"
#include "utils/Logger.hpp"
#include "utils/Helpers.hpp"

// ── Global state for signal handler ──────────────────────────────
static std::vector<BufferManager*> g_managers;
static volatile sig_atomic_t g_running = 1;

// SIGINT handler — sets flag to break main loop
void sigintHandler(int /*sig*/) {
    g_running = 0;
}

int main() {
    // ── Register signal handlers ─────────────────────────────────
    std::signal(SIGINT, sigintHandler);
    std::signal(SIGTERM, sigintHandler);

    Logger::info("=== DBMS Buffer Manager Simulation ===");
    Logger::info("Buffer size: " + std::to_string(Config::BUFFER_SIZE) + " frames");
    Logger::info("Page size: " + std::to_string(Config::PAGE_SIZE) + " records/page");
    Logger::info("Polling interval: " + std::to_string(Config::POLL_INTERVAL_MS) + " ms");
    Logger::info("Press Ctrl+C to stop and dump metrics.");

    // ── Initialize components ────────────────────────────────────

    // Create SQLite database manager
    SQLiteManager dbManager(Config::DB_PATH);
    if (!dbManager.isOpen()) {
        Logger::error("Failed to open database. Exiting.");
        return 1;
    }

    // Create file watcher for query.sql
    FileWatcher watcher(Config::QUERY_FILE);

    // Create all 4 buffer managers
    LRUBuffer    lruBuffer(Config::BUFFER_SIZE);
    MRUBuffer    mruBuffer(Config::BUFFER_SIZE);
    ClockBuffer  clockBuffer(Config::BUFFER_SIZE);
    PinnedBuffer pinnedBuffer(Config::BUFFER_SIZE);

    g_managers = {&lruBuffer, &mruBuffer, &clockBuffer, &pinnedBuffer};

    Logger::info("All buffer managers initialized. Watching for queries...\n");

    // ── Main loop ────────────────────────────────────────────────
    while (g_running) {
        if (watcher.hasChanged()) {
            // Read the SQL query from file
            std::string sql = watcher.readFile();

            // Trim whitespace
            while (!sql.empty() && (sql.back() == '\n' || sql.back() == '\r' || sql.back() == ' ')) {
                sql.pop_back();
            }

            if (sql.empty()) {
                Logger::warn("query.sql is empty. Skipping.");
                continue;
            }

            Logger::info("New query detected: " + sql);

            // Execute the query and get record count
            int recordCount = dbManager.executeQuery(sql);

            if (recordCount <= 0) {
                Logger::warn("Query returned 0 records. Skipping buffer simulation.");
                continue;
            }

            // Generate page access sequence
            std::vector<int> pageSequence = Helpers::generatePageSequence(
                recordCount, Config::PAGE_SIZE);

            Logger::info("Page sequence generated: " + std::to_string(pageSequence.size()) + " pages");

            // Feed page sequence to all buffer managers
            for (BufferManager* mgr : g_managers) {
                for (int pageId : pageSequence) {
                    mgr->accessPage(pageId);
                }
            }

            // Print quick summary to stdout
            Logger::info("--- Quick Metrics Summary ---");
            for (BufferManager* mgr : g_managers) {
                Metrics m = mgr->getMetrics();
                Logger::info(m.strategyName + " | Hits: " + std::to_string(m.bufferHits)
                    + " | Misses: " + std::to_string(m.bufferMisses)
                    + " | Evictions: " + std::to_string(m.evictions));
            }
            Logger::info("-----------------------------\n");
        }

        // Sleep for polling interval
        std::this_thread::sleep_for(
            std::chrono::milliseconds(Config::POLL_INTERVAL_MS));
    }

    // ── SIGINT received — dump metrics to file ───────────────────
    Logger::info("\nSIGINT received. Dumping metrics to " + Config::METRICS_FILE + "...");

    Helpers::dumpMetricsToFile(g_managers, Config::METRICS_FILE);

    Logger::info("Metrics written to " + Config::METRICS_FILE);
    Logger::info("Goodbye!");

    return 0;
}
