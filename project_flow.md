You are building the BACKEND module of a DBMS Buffer Manager Simulation project.

⚠️ STRICT REQUIREMENTS:
- Language: C++
- Use SQLite C/C++ library
- Code must be modular using .hpp and .cpp files
- Entire backend must be inside a /backend folder
- Follow clean architecture and separation of concerns
- No monolithic code

--------------------------------------------------

🎯 OBJECTIVE:

Simulate a buffer pool for database queries using multiple replacement strategies:
- LRU
- MRU
- CLOCK
- PINNED BLOCKS

The system continuously listens to a SQL file and processes queries in real-time.

--------------------------------------------------

📂 INPUT PIPELINE:

- There exists a file: `/backend/input/query.sql`
- This file ALWAYS contains exactly ONE SQL query
- It is overwritten by the frontend whenever a new query is issued

--------------------------------------------------

🔁 PROGRAM BEHAVIOR:

- A C++ backend process runs continuously
- It monitors the query.sql file for changes (polling or file timestamp check)
- When the file changes:
    1. Read the SQL query
    2. Send it to SQLite
    3. Extract PAGE ACCESS SEQUENCE from execution

--------------------------------------------------

📄 PAGE MODEL:

- Each page contains 5 records
- You must simulate page IDs (not actual SQLite pages)
- Convert query result → page numbers
- Example:
    If 23 records accessed → pages = ceil(23 / 5)

--------------------------------------------------

🧠 BUFFER MANAGER:

Implement 4 independent buffer managers:

1. LRUBuffer
2. MRUBuffer
3. ClockBuffer
4. PinnedBuffer

Each has:
- Fixed buffer size (configurable)
- Its OWN buffer pool (no sharing)

--------------------------------------------------

⚙️ PROCESSING LOGIC (FOR EACH STRATEGY):

For each page in page access sequence:
    IF page is in buffer:
        → buffer hit
    ELSE:
        → buffer miss
        → increment disk I/O counter
        → bring page into buffer using replacement policy

--------------------------------------------------

📊 METRICS TO TRACK:

For EACH strategy:

MANDATORY:
- Total Requests
- Buffer Hits
- Buffer Misses
- Disk Reads (same as misses)
- Hit Ratio
- Miss Ratio

BONUS (add these):
- Evictions count
- Average reuse distance (if possible)
- Time taken per query
- Throughput (queries/sec)

--------------------------------------------------

📁 OUTPUT:

Write results to:
`/backend/output/metrics.txt`

Format should be clean and comparable:

Example:

Strategy: LRU
Requests: 100
Hits: 60
Misses: 40
Disk Reads: 40
Hit Ratio: 0.60
Evictions: 35

------------------------

Do this for ALL strategies.

--------------------------------------------------

🧱 PROJECT STRUCTURE:

backend/
│── Makefile
├── main.cpp
├── config/
│   └── Config.hpp
│
├── watcher/
│   ├── FileWatcher.hpp
│   └── FileWatcher.cpp
│
├── database/
│   ├── SQLiteManager.hpp
│   └── SQLiteManager.cpp
│
├── buffer/
│   ├── BufferManager.hpp   (abstract base class)
│   ├── LRUBuffer.hpp
│   ├── LRUBuffer.cpp
│   ├── MRUBuffer.hpp
│   ├── MRUBuffer.cpp
│   ├── ClockBuffer.hpp
│   ├── ClockBuffer.cpp
│   ├── PinnedBuffer.hpp
│   ├── PinnedBuffer.cpp
│
├── models/
│   ├── Page.hpp
│   └── Metrics.hpp
│
├── utils/
│   ├── Logger.hpp
│   └── Helpers.hpp
│
├── input/
│   └── query.sql
│
├── output/
│   └── metrics.txt
│
└── CMakeLists.txt

--------------------------------------------------

🧩 CLASS DESIGN REQUIREMENTS:

1. BufferManager (abstract class)
    - virtual void accessPage(int pageId) = 0;
    - virtual Metrics getMetrics() = 0;

2. Each strategy implements BufferManager

3. SQLiteManager:
    - Executes query
    - Returns number of records accessed
    - Converts to page sequence

4. FileWatcher:
    - Detects file changes
    - Triggers processing

--------------------------------------------------

🔁 MAIN LOOP:

while(true):
    if query.sql changed:
        read query
        execute query via SQLite
        generate page sequence
        send sequence to ALL 4 buffer managers
        collect metrics
        write to metrics.txt

--------------------------------------------------

⚠️ IMPLEMENTATION NOTES:

- Use unordered_map + list for LRU
- Use stack-like behavior for MRU
- Use circular pointer for CLOCK
- PinnedBuffer:
    - Some pages cannot be evicted (simulate pinned logic)

- Avoid actual disk I/O → simulate via counters

- Use thread-safe file reading if needed

--------------------------------------------------

🧪 EXTRA:

- Add config for:
    - buffer size
    - page size (default 5 records)
    - polling interval

--------------------------------------------------

🎯 OUTPUT QUALITY:

- Clean, readable, modular code
- Proper header/source separation
- Comments explaining logic
- No unnecessary complexity

--------------------------------------------------

Generate COMPLETE working backend code with all files.