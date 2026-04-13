#!/bin/bash
# ─────────────────────────────────────────────────
# Test script for Buffer Manager
# Run ./setup_db.sh FIRST to populate the database
# Then start ./buffer_manager in one terminal
# Run this in a SEPARATE terminal
# ─────────────────────────────────────────────────

QUERY_FILE="input/query.sql"
DELAY=2  # seconds between queries

echo "=========================================="
echo "  Buffer Manager Test Sequence"
echo "  Buffer: 10 frames | Page: 5 records/page"
echo "=========================================="
echo ""

# ── Q1: Small query — cold start ──
echo "[Q1] SELECT * FROM courses WHERE department='Computer Science';  (~14 records → 3 pages)"
echo "     → Expected: all misses (cold start)"
echo "SELECT * FROM courses WHERE department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q2: Overlapping pages — test HITS ──
echo "[Q2] SELECT * FROM courses WHERE credits=4;  (~15 records → 3 pages)"
echo "     → Expected: some hits (pages 0,1,2 likely cached)"
echo "SELECT * FROM courses WHERE credits=4;" > "$QUERY_FILE"
sleep $DELAY

# ── Q3: Medium query — fill buffer more ──
echo "[Q3] SELECT * FROM students WHERE department='Computer Science';  (~40 records → 8 pages)"
echo "     → Expected: more misses, buffer filling up"
echo "SELECT * FROM students WHERE department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q4: Full table scan — OVERFLOW buffer → EVICTIONS ──
echo "[Q4] SELECT * FROM students;  (100 records → 20 pages)"
echo "     → Expected: 20 page accesses, heavy evictions (buffer = 10)"
echo "     → Strategies start DIVERGING here!"
echo "SELECT * FROM students;" > "$QUERY_FILE"
sleep $DELAY

# ── Q5: Repeat Q3 — test post-eviction retention ──
echo "[Q5] SELECT * FROM students WHERE department='Computer Science';  (repeat)"
echo "     → Expected: MRU retains more than LRU after full scan"
echo "SELECT * FROM students WHERE department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q6: JOIN query — lots of records ──
echo "[Q6] SELECT e.*, s.name, c.title FROM enrollments e JOIN students s ON e.student_id=s.id JOIN courses c ON e.course_id=c.id WHERE e.grade='A';  (~big result)"
echo "     → Expected: massive page access, more evictions"
echo "SELECT e.*, s.name, c.title FROM enrollments e JOIN students s ON e.student_id=s.id JOIN courses c ON e.course_id=c.id WHERE e.grade='A';" > "$QUERY_FILE"
sleep $DELAY

# ── Q7: Different table — library ──
echo "[Q7] SELECT * FROM library_books WHERE genre='Textbook';  (~25 records → 5 pages)"
echo "     → Expected: all new pages, more evictions"
echo "SELECT * FROM library_books WHERE genre='Textbook';" > "$QUERY_FILE"
sleep $DELAY

# ── Q8: Small targeted query ──
echo "[Q8] SELECT * FROM enrollments WHERE year=2026 AND marks > 85;  (small result)"
echo "     → Expected: few pages, check hit rates"
echo "SELECT * FROM enrollments WHERE year=2026 AND marks > 85;" > "$QUERY_FILE"
sleep $DELAY

# ── Q9: Full scan on big table — stress test ──
echo "[Q9] SELECT * FROM enrollments;  (200 records → 40 pages)"
echo "     → Expected: MAX evictions, buffer thrashing"
echo "SELECT * FROM enrollments;" > "$QUERY_FILE"
sleep $DELAY

# ── Q10: Repeat small query — final retention test ──
echo "[Q10] SELECT * FROM library_books WHERE genre='AI/ML';  (small result → 2 pages)"
echo "      → Expected: shows final retention behavior per strategy"
echo "SELECT * FROM library_books WHERE genre='AI/ML';" > "$QUERY_FILE"
sleep $DELAY

echo ""
echo "=========================================="
echo "  All 10 queries sent!"
echo "  Now press Ctrl+C in the buffer_manager"
echo "  terminal to dump final metrics."
echo "  Then check: cat output/metrics.txt"
echo "=========================================="
