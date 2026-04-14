SELECT s.name, c.title, e.marks FROM students s JOIN enrollments e ON s.id=e.student_id JOIN courses c ON e.course_id=c.id WHERE e.marks > 95;
