# Authors
- Kaustav Mishra (23CS10030)
- Rohit Ranjeet Satpute (23CS10060)
- Tanishq Sura (23CS10071)
- Aditya Prakash (23CS10088)
- Tanmay Nitin Amritkar (23CS30066)

# Simulation and Analysis of Buffer Manager Strategies with a Natural Language Query Interface

Term Project, Database Management Systems, IIT Kharagpur (Spring 2025-26)

## Project Overview

This project simulates and compares multiple buffer replacement strategies under SQL workloads.
It combines:

- A C++ backend that executes SQL queries on SQLite and simulates page accesses.
- Four buffer manager policies: LRU, MRU, CLOCK (Second-Chance), and PINNED.
- A lightweight Natural Language to SQL interface (Streamlit + Ollama API) for user-friendly query input.

The main goal is to observe how different replacement policies behave in terms of hit ratio, miss ratio, disk reads, and evictions when query patterns change.

## Key Features

- SQLite-backed query execution with real table data.
- File-watcher driven simulation loop (reacts to new SQL in `backend/input/query.sql`).
- Page sequence generation from query result size using configurable page size.
- Comparative metrics collection across all strategies for the same workload.
- Query result export to `backend/output/display.txt`.
- Metrics report export to `backend/output/metrics.txt` on shutdown.

## Implemented Buffer Policies

- LRU (Least Recently Used): evicts the least recently used page.
- MRU (Most Recently Used): evicts the most recently used page.
- CLOCK (Second-Chance): uses a circular frame list and reference bits to approximate LRU efficiently.
- PINNED: LRU-style eviction with non-evictable pinned pages (simulation heuristic: every 7th page is pinned).

## System Workflow

1. A SQL query is written to `backend/input/query.sql`.
2. Backend detects file change and executes query on SQLite database.
3. Returned record count is converted into a page access sequence.
4. The sequence is fed to all four buffer managers.
5. Running summary is logged; latest query result is written to `backend/output/display.txt`.
6. On `Ctrl+C`, final metrics are dumped to `backend/output/metrics.txt`.

For the natural-language path:

1. User types English prompt in Streamlit UI.
2. Prompt is saved to `frontend/user_prompt.txt`.
3. `frontend/text_to_sql.py` calls local Ollama API and writes SQL to `backend/input/query.sql`.
4. Backend processes query and writes results to `backend/output/display.txt`.

## Repository Structure

```
backend/
	main.cpp                  # Simulation entry point
	Makefile                  # Build rules for backend binary
	setup_db.sh               # Creates and populates SQLite database
	test.sh                   # Sends a predefined SQL workload
	buffer/                   # LRU, MRU, CLOCK, PINNED implementations
	database/                 # SQLite manager and query execution
	watcher/                  # File change detection for query input
	config/                   # Global simulation constants
	input/query.sql           # Incoming SQL workload file
	output/                   # display.txt and metrics.txt

frontend/
	app.py                    # Streamlit chat-style interface
	text_to_sql.py            # English-to-SQL generation via Ollama
	user_prompt.txt           # Latest NL prompt from UI
	requirements.txt
```

## Prerequisites

- Linux environment
- C++ toolchain (`g++`, `gcc`, `make`)
- SQLite CLI (`sqlite3`) for database setup script
- Python
- Streamlit (for UI)
- Ollama running locally (for NL to SQL conversion)

## Quick Start

### 1) Setup and build backend

```bash
cd backend
chmod +x setup_db.sh test.sh
./setup_db.sh
make
```

### 2) Run backend simulator

```bash
cd backend
./buffer_manager
```

Backend now watches `backend/input/query.sql` continuously.

### 3A) Run predefined SQL workload (recommended for evaluation)

In a second terminal:

```bash
cd backend
./test.sh
```

After workload completes, press `Ctrl+C` in the backend terminal to generate final metrics.

### 3B) Run natural language interface (optional)

In a separate terminal:

```bash
cd frontend
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
streamlit run app.py
```

Then convert prompts to SQL (same terminal or another terminal inside `frontend`):

```bash
python text_to_sql.py
```

Note: The current NL-to-SQL bridge is file-based and semi-manual. Run `text_to_sql.py` after entering a new prompt in the UI.

## Output Files

- `backend/output/display.txt`: latest executed query, timestamp, record count, and formatted result table.
- `backend/output/metrics.txt`: final per-strategy metrics (total requests, hits, misses, disk reads, hit ratio, miss ratio, evictions).

## Configuration

Simulation parameters are defined in `backend/config/Config.hpp`:

- Buffer size: 10 frames
- Page size: 5 records/page
- Poll interval: 500 ms

You can modify these constants and rebuild to run new experiments.

## Notes and Current Limitations

- Page access simulation is derived from query result size, not physical DB page layout.
- For multi-table queries, base page mapping is anchored to the first table after `FROM`.
- Natural language path currently requires running `text_to_sql.py` explicitly for each prompt.

## Suggested Demo Flow

1. Run backend and execute `test.sh`.
2. Observe live quick metrics in terminal logs.
3. Interrupt backend with `Ctrl+C`.
4. Compare strategies from `backend/output/metrics.txt`.

This gives a reproducible baseline for discussing policy behavior and workload sensitivity.
