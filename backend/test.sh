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
echo "  Buffer Manager Test Sequence (35 Queries)"
echo "  Buffer: 10 frames | Page: 5 records/page"
echo "=========================================="
echo ""

# ── Q1 ──
echo "[Q1] SELECT * FROM courses WHERE department='Computer Science';"
echo "     → Expected: all misses (cold start)"
echo "SELECT * FROM courses WHERE department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q2 ──
echo "[Q2] SELECT * FROM courses WHERE credits=4;"
echo "     → Expected: some hits (pages 0,1,2 likely cached)"
echo "SELECT * FROM courses WHERE credits=4;" > "$QUERY_FILE"
sleep $DELAY

# ── Q3 ──
echo "[Q3] SELECT * FROM students WHERE department='Computer Science';"
echo "     → Expected: more misses, buffer filling up"
echo "SELECT * FROM students WHERE department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q4 ──
echo "[Q4] SELECT * FROM students;"
echo "     → Expected: 20 page accesses, heavy evictions (buffer = 10)"
echo "SELECT * FROM students;" > "$QUERY_FILE"
sleep $DELAY

# ── Q5 ──
echo "[Q5] SELECT * FROM students WHERE department='Computer Science';"
echo "     → Expected: MRU retains more than LRU after full scan"
echo "SELECT * FROM students WHERE department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q6 ──
echo "[Q6] SELECT e.*, s.name, c.title FROM enrollments e JOIN students s ON e.student_id=s.id JOIN courses c ON e.course_id=c.id WHERE e.grade='A';"
echo "     → Expected: massive page access, more evictions"
echo "SELECT e.*, s.name, c.title FROM enrollments e JOIN students s ON e.student_id=s.id JOIN courses c ON e.course_id=c.id WHERE e.grade='A';" > "$QUERY_FILE"
sleep $DELAY

# ── Q7 ──
echo "[Q7] SELECT * FROM library_books WHERE genre='Textbook';"
echo "     → Expected: all new pages, more evictions from library_books"
echo "SELECT * FROM library_books WHERE genre='Textbook';" > "$QUERY_FILE"
sleep $DELAY

# ── Q8 ──
echo "[Q8] SELECT * FROM enrollments WHERE year=2026 AND marks > 85;"
echo "     → Expected: few pages, check hit rates"
echo "SELECT * FROM enrollments WHERE year=2026 AND marks > 85;" > "$QUERY_FILE"
sleep $DELAY

# ── Q9 ──
echo "[Q9] SELECT * FROM enrollments;"
echo "     → Expected: MAX evictions, buffer thrashing"
echo "SELECT * FROM enrollments;" > "$QUERY_FILE"
sleep $DELAY

# ── Q10 ──
echo "[Q10] SELECT * FROM library_books WHERE genre='AI/ML';"
echo "      → Expected: shows final retention behavior per strategy"
echo "SELECT * FROM library_books WHERE genre='AI/ML';" > "$QUERY_FILE"
sleep $DELAY

# ── Q11 ──
echo "[Q11] SELECT s.name, e.grade FROM students s JOIN enrollments e ON s.id=e.student_id WHERE s.department='Computer Science';"
echo "      → Expected: JOIN between two tables, heavy access"
echo "SELECT s.name, e.grade FROM students s JOIN enrollments e ON s.id=e.student_id WHERE s.department='Computer Science';" > "$QUERY_FILE"
sleep $DELAY

# ── Q12 ──
echo "[Q12] SELECT * FROM library_books WHERE published_year > 2010;"
echo "      → Expected: moderate scan on library table"
echo "SELECT * FROM library_books WHERE published_year > 2010;" > "$QUERY_FILE"
sleep $DELAY

# ── Q13 ──
echo "[Q13] SELECT c.title, e.marks FROM courses c JOIN enrollments e ON c.id=e.course_id WHERE c.credits=3;"
echo "      → Expected: JOIN filtering on course attributes"
echo "SELECT c.title, e.marks FROM courses c JOIN enrollments e ON c.id=e.course_id WHERE c.credits=3;" > "$QUERY_FILE"
sleep $DELAY

# ── Q14 ──
echo "[Q14] SELECT * FROM students WHERE cgpa > 9.0;"
echo "      → Expected: sequential scan filtering on high CGPA"
echo "SELECT * FROM students WHERE cgpa > 9.0;" > "$QUERY_FILE"
sleep $DELAY

# ── Q15 ──
echo "[Q15] SELECT * FROM library_books WHERE author='Silberschatz';"
echo "      → Expected: scan matching specific author"
echo "SELECT * FROM library_books WHERE author='Silberschatz';" > "$QUERY_FILE"
sleep $DELAY

# ── Q16 ──
echo "[Q16] SELECT s.name, c.title, e.marks FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE c.credits=4;"
echo "      → Expected: triple JOIN heavily stressing buffer"
echo "SELECT s.name, c.title, e.marks FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE c.credits=4;" > "$QUERY_FILE"
sleep $DELAY

# ── Q17 ──
echo "[Q17] SELECT * FROM enrollments WHERE marks >= 80 AND marks <= 90;"
echo "      → Expected: range filter on enrollments"
echo "SELECT * FROM enrollments WHERE marks >= 80 AND marks <= 90;" > "$QUERY_FILE"
sleep $DELAY

# ── Q18 ──
echo "[Q18] SELECT s.name, e.attendance_pct FROM students s JOIN enrollments e ON s.id=e.student_id WHERE e.grade='A+';"
echo "      → Expected: JOIN checking exact match on grade"
echo "SELECT s.name, e.attendance_pct FROM students s JOIN enrollments e ON s.id=e.student_id WHERE e.grade='A+';" > "$QUERY_FILE"
sleep $DELAY

# ── Q19 ──
echo "[Q19] SELECT * FROM courses WHERE max_seats > 50;"
echo "      → Expected: filter on small table, potential hits"
echo "SELECT * FROM courses WHERE max_seats > 50;" > "$QUERY_FILE"
sleep $DELAY

# ── Q20 ──
echo "[Q20] SELECT * FROM library_books WHERE available_copies < 3;"
echo "      → Expected: small scan on library table"
echo "SELECT * FROM library_books WHERE available_copies < 3;" > "$QUERY_FILE"
sleep $DELAY

# ── Q21 ──
echo "[Q21] SELECT * FROM students WHERE semester=8;"
echo "      → Expected: frequent access to students table"
echo "SELECT * FROM students WHERE semester=8;" > "$QUERY_FILE"
sleep $DELAY

# ── Q22 ──
echo "[Q22] SELECT * FROM enrollments WHERE attendance_pct < 80.0;"
echo "      → Expected: filter on enrollments, likely hits if retained"
echo "SELECT * FROM enrollments WHERE attendance_pct < 80.0;" > "$QUERY_FILE"
sleep $DELAY

# ── Q23 ──
echo "[Q23] SELECT s.name, c.title FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE c.department='Mechanical';"
echo "      → Expected: complex JOIN for specific department"
echo "SELECT s.name, c.title FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE c.department='Mechanical';" > "$QUERY_FILE"
sleep $DELAY

# ── Q24 ──
echo "[Q24] SELECT * FROM library_books WHERE genre='Software';"
echo "      → Expected: filter on library books by genre"
echo "SELECT * FROM library_books WHERE genre='Software';" > "$QUERY_FILE"
sleep $DELAY

# ── Q25 ──
echo "[Q25] SELECT e.year, s.cgpa FROM students s JOIN enrollments e ON s.id=e.student_id WHERE s.semester=6;"
echo "      → Expected: JOIN between students and enrollments"
echo "SELECT e.year, s.cgpa FROM students s JOIN enrollments e ON s.id=e.student_id WHERE s.semester=6;" > "$QUERY_FILE"
sleep $DELAY

# ── Q26 ──
echo "[Q26] SELECT * FROM courses;"
echo "      → Expected: full scan on courses (small table)"
echo "SELECT * FROM courses;" > "$QUERY_FILE"
sleep $DELAY

# ── Q27 ──
echo "[Q27] SELECT * FROM library_books WHERE total_copies > 5;"
echo "      → Expected: filter on library books by total copies"
echo "SELECT * FROM library_books WHERE total_copies > 5;" > "$QUERY_FILE"
sleep $DELAY

# ── Q28 ──
echo "[Q28] SELECT c.code, e.marks FROM enrollments e JOIN courses c ON e.course_id=c.id WHERE e.year=2024;"
echo "      → Expected: JOIN on enrollments and courses by year"
echo "SELECT c.code, e.marks FROM enrollments e JOIN courses c ON e.course_id=c.id WHERE e.year=2024;" > "$QUERY_FILE"
sleep $DELAY

# ── Q29 ──
echo "[Q29] SELECT * FROM students WHERE department='Electronics' AND cgpa > 8.0;"
echo "      → Expected: filter on students combining attributes"
echo "SELECT * FROM students WHERE department='Electronics' AND cgpa > 8.0;" > "$QUERY_FILE"
sleep $DELAY

# ── Q30 ──
echo "[Q30] SELECT s.roll_no, c.title, e.grade FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE c.department='Civil';"
echo "      → Expected: multi-table JOIN for Civil engineering"
echo "SELECT s.roll_no, c.title, e.grade FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE c.department='Civil';" > "$QUERY_FILE"
sleep $DELAY

# ── Q31 ──
echo "[Q31] SELECT * FROM enrollments WHERE grade='B';"
echo "      → Expected: filter on enrollments by grade"
echo "SELECT * FROM enrollments WHERE grade='B';" > "$QUERY_FILE"
sleep $DELAY

# ── Q32 ──
echo "[Q32] SELECT * FROM library_books WHERE published_year > 2000 AND published_year < 2010;"
echo "      → Expected: filter on library table by date range"
echo "SELECT * FROM library_books WHERE published_year > 2000 AND published_year < 2010;" > "$QUERY_FILE"
sleep $DELAY

# ── Q33 ──
echo "[Q33] SELECT * FROM students;"
echo "      → Expected: full table scan on students for eviction stress"
echo "SELECT * FROM students;" > "$QUERY_FILE"
sleep $DELAY

# ── Q34 ──
echo "[Q34] SELECT s.name, e.course_id FROM students s JOIN enrollments e ON s.id=e.student_id WHERE e.marks < 75;"
echo "      → Expected: JOIN identifying low performing records"
echo "SELECT s.name, e.course_id FROM students s JOIN enrollments e ON s.id=e.student_id WHERE e.marks < 75;" > "$QUERY_FILE"
sleep $DELAY

# ── Q35 ──
echo "[Q35] SELECT s.name, c.title, e.marks FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE e.marks > 95;"
echo "      → Expected: final massive JOIN for top performances"
echo "SELECT s.name, c.title, e.marks FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE e.marks > 95;" > "$QUERY_FILE"
sleep $DELAY

echo ""
echo "=========================================="
echo "  All 35 queries sent!"
echo "  Now press Ctrl+C in the buffer_manager"
echo "  terminal to dump final metrics."
echo "  Then check: cat output/metrics.txt"
echo "=========================================="
