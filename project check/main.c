#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define DB_DIR "/data/"
#else
#define EMSCRIPTEN_KEEPALIVE
#define DB_DIR ""
#endif

#define MAX_STUDENTS 200
#define MAX_TEACHERS 50
#define MAX_COURSES  40
#define MAX_MARKS    2000
#define MAX_ATT      2000

typedef struct { char id[16], pass[24], name[50], program[30]; int semester, active; } Student;
typedef struct { char id[16], pass[24], name[50], dept[30]; } Teacher;
typedef struct { char code[12], title[40]; int credits; char teacherId[16]; } Course;
typedef struct { char studentId[16], courseCode[12]; float marks; } Mark;
typedef struct { char studentId[16], courseCode[12]; int total; int attended; } Attendance;

Student students[MAX_STUDENTS]; int studentCount = 0;
Teacher teachers[MAX_TEACHERS]; int teacherCount = 0;
Course  courses [MAX_COURSES ]; int courseCount  = 0;
Mark    marks   [MAX_MARKS   ]; int markCount    = 0;
Attendance atts [MAX_ATT     ]; int attCount     = 0;

static void copyStr(char *dst, size_t n, const char *src) { strncpy(dst, src, n - 1); dst[n - 1] = '\0'; }
static int findStudent(const char *id) { for (int i = 0; i < studentCount; i++) if (strcmp(students[i].id, id) == 0) return i; return -1; }
static int findTeacher(const char *id) { for (int i = 0; i < teacherCount; i++) if (strcmp(teachers[i].id, id) == 0) return i; return -1; }
static int findCourse(const char *code) { for (int i = 0; i < courseCount; i++) if (strcmp(courses[i].code, code) == 0) return i; return -1; }
static int findMark(const char *sid, const char *ccode) { for (int i = 0; i < markCount; i++) if (strcmp(marks[i].studentId, sid) == 0 && strcmp(marks[i].courseCode, ccode) == 0) return i; return -1; }
static int findAtt(const char *sid, const char *ccode) { for (int i = 0; i < attCount; i++) if (strcmp(atts[i].studentId, sid) == 0 && strcmp(atts[i].courseCode, ccode) == 0) return i; return -1; }

static float gradePoint(float m) {
    if (m >= 85) return 4.0f; if (m >= 80) return 3.7f; if (m >= 75) return 3.3f;
    if (m >= 70) return 3.0f; if (m >= 65) return 2.7f; if (m >= 60) return 2.3f;
    if (m >= 55) return 2.0f; if (m >= 50) return 1.7f; if (m >= 45) return 1.3f;
    return 0.0f;
}

void loadAll() {
    FILE *f; char line[400];
    f = fopen(DB_DIR "students.txt", "r"); if (f) { studentCount = 0; while (fgets(line, sizeof line, f)) { line[strcspn(line, "\r\n")] = 0; if(!line[0]) continue; Student s; char *t = strtok(line, "|"); if(!t) continue; copyStr(s.id, 16, t); t=strtok(NULL,"|"); copyStr(s.pass,24,t); t=strtok(NULL,"|"); copyStr(s.name,50,t); t=strtok(NULL,"|"); copyStr(s.program,30,t); t=strtok(NULL,"|"); s.semester=atoi(t); t=strtok(NULL,"|"); s.active=t?atoi(t):1; students[studentCount++]=s; } fclose(f); }
    f = fopen(DB_DIR "teachers.txt", "r"); if (f) { teacherCount = 0; while (fgets(line, sizeof line, f)) { line[strcspn(line, "\r\n")] = 0; if(!line[0]) continue; Teacher x; char *t = strtok(line, "|"); if(!t) continue; copyStr(x.id,16,t); t=strtok(NULL,"|"); copyStr(x.pass,24,t); t=strtok(NULL,"|"); copyStr(x.name,50,t); t=strtok(NULL,"|"); copyStr(x.dept,30,t); teachers[teacherCount++]=x; } fclose(f); }
    f = fopen(DB_DIR "courses.txt", "r"); if (f) { courseCount = 0; while (fgets(line, sizeof line, f)) { line[strcspn(line, "\r\n")] = 0; if(!line[0]) continue; Course c; char *t = strtok(line, "|"); if(!t) continue; copyStr(c.code,12,t); t=strtok(NULL,"|"); copyStr(c.title,40,t); t=strtok(NULL,"|"); c.credits=atoi(t); t=strtok(NULL,"|"); copyStr(c.teacherId,16,t); courses[courseCount++]=c; } fclose(f); }
    f = fopen(DB_DIR "marks.txt", "r"); if (f) { markCount = 0; while (fgets(line, sizeof line, f)) { line[strcspn(line, "\r\n")] = 0; if(!line[0]) continue; Mark m; char *t = strtok(line, "|"); if(!t) continue; copyStr(m.studentId,16,t); t=strtok(NULL,"|"); copyStr(m.courseCode,12,t); t=strtok(NULL,"|"); m.marks=atof(t); marks[markCount++]=m; } fclose(f); }
    f = fopen(DB_DIR "attendance.txt", "r"); if (f) { attCount = 0; while (fgets(line, sizeof line, f)) { line[strcspn(line, "\r\n")] = 0; if(!line[0]) continue; Attendance a; char *t = strtok(line, "|"); if(!t) continue; copyStr(a.studentId,16,t); t=strtok(NULL,"|"); copyStr(a.courseCode,12,t); t=strtok(NULL,"|"); a.total=atoi(t); t=strtok(NULL,"|"); a.attended=atoi(t); atts[attCount++]=a; } fclose(f); }
}

void saveStudents() { FILE *f = fopen(DB_DIR "students.txt", "w"); for (int i = 0; i < studentCount; i++) fprintf(f, "%s|%s|%s|%s|%d|%d\n", students[i].id, students[i].pass, students[i].name, students[i].program, students[i].semester, students[i].active); fclose(f); }
void saveTeachers() { FILE *f = fopen(DB_DIR "teachers.txt", "w"); for (int i = 0; i < teacherCount; i++) fprintf(f, "%s|%s|%s|%s\n", teachers[i].id, teachers[i].pass, teachers[i].name, teachers[i].dept); fclose(f); }
void saveCourses() { FILE *f = fopen(DB_DIR "courses.txt", "w"); for (int i = 0; i < courseCount; i++) fprintf(f, "%s|%s|%d|%s\n", courses[i].code, courses[i].title, courses[i].credits, courses[i].teacherId); fclose(f); }
void saveMarks() { FILE *f = fopen(DB_DIR "marks.txt", "w"); for (int i = 0; i < markCount; i++) fprintf(f, "%s|%s|%.1f\n", marks[i].studentId, marks[i].courseCode, marks[i].marks); fclose(f); }
void saveAtts() { FILE *f = fopen(DB_DIR "attendance.txt", "w"); for (int i = 0; i < attCount; i++) fprintf(f, "%s|%s|%d|%d\n", atts[i].studentId, atts[i].courseCode, atts[i].total, atts[i].attended); fclose(f); }

void seedData() {
    FILE *f = fopen(DB_DIR "students.txt", "r"); if (!f) { f = fopen(DB_DIR "students.txt", "w"); fprintf(f, "STU001|1234|Ali Khan|BSCS|1|1\n"); fclose(f); } else fclose(f);
    f = fopen(DB_DIR "teachers.txt", "r"); if (!f) { f = fopen(DB_DIR "teachers.txt", "w"); fprintf(f, "TCH001|1234|Dr. Imran|CS\n"); fclose(f); } else fclose(f);
    f = fopen(DB_DIR "courses.txt", "r"); if (!f) { f = fopen(DB_DIR "courses.txt", "w"); fprintf(f, "CS101|Programming Fundamentals|3|TCH001\n"); fclose(f); } else fclose(f);
    f = fopen(DB_DIR "marks.txt", "r"); if (!f) { f = fopen(DB_DIR "marks.txt", "w"); fprintf(f, "STU001|CS101|78.5\n"); fclose(f); } else fclose(f);
    f = fopen(DB_DIR "attendance.txt", "r"); if (!f) { f = fopen(DB_DIR "attendance.txt", "w"); fprintf(f, "STU001|CS101|10|9\n"); fclose(f); } else fclose(f);
}

/* =========================================================
   BRIDGE FUNCTIONS
   ========================================================= */

EMSCRIPTEN_KEEPALIVE
void api_init_db() { seedData(); loadAll(); }

EMSCRIPTEN_KEEPALIVE
int api_login(const char* role, const char* id, const char* pass) {
    if (strcmp(role, "admin") == 0) return (strcmp(id, "admin") == 0 && strcmp(pass, "admin123") == 0) ? 1 : 0;
    if (strcmp(role, "student") == 0) { int i = findStudent(id); if (i >= 0 && students[i].active && strcmp(students[i].pass, pass) == 0) return i + 1; }
    if (strcmp(role, "teacher") == 0) { int i = findTeacher(id); if (i >= 0 && strcmp(teachers[i].pass, pass) == 0) return i + 1; }
    return 0;
}

EMSCRIPTEN_KEEPALIVE
const char* api_get_student_result(const char* id) {
    static char output[4096]; output[0] = '\0'; int si = findStudent(id); if (si < 0) return "Student not found";
    float totalQP = 0; int totalCr = 0; char buf[300];
    sprintf(output, "<h3>Result Card: %s (%s)</h3>", students[si].name, students[si].program);
    strcat(output, "<table><tr><th>Code</th><th>Course</th><th>Marks</th><th>Grade</th></tr>");
    for (int i = 0; i < markCount; i++) {
        if (strcmp(marks[i].studentId, id) == 0) {
            int c = findCourse(marks[i].courseCode);
            if (c >= 0) {
                float m = marks[i].marks;
                char grade = (m >= 85) ? 'A' : (m >= 75) ? 'B' : (m >= 65) ? 'C' : (m >= 50) ? 'D' : 'F';
                sprintf(buf, "<tr><td>%s</td><td>%s</td><td>%.1f</td><td>%c</td></tr>", courses[c].code, courses[c].title, m, grade);
                strcat(output, buf); totalQP += gradePoint(m) * courses[c].credits; totalCr += courses[c].credits;
            }
        }
    }
    strcat(output, "</table>");
    if (totalCr > 0) { sprintf(buf, "<div class='summary'><b>Credits:</b> %d | <b>GPA:</b> %.2f</div>", totalCr, totalQP / totalCr); strcat(output, buf); }
    return output;
}

EMSCRIPTEN_KEEPALIVE
const char* api_get_student_attendance_json(const char* id) {
    static char output[4096]; output[0] = '\0'; strcat(output, "["); int first = 1;
    for (int i = 0; i < attCount; i++) {
        if (strcmp(atts[i].studentId, id) == 0) {
            if (!first) strcat(output, ","); first = 0;
            float percent = atts[i].total > 0 ? ((float)atts[i].attended / atts[i].total) * 100.0f : 0;
            char buf[200];
            sprintf(buf, "{\"course\":\"%s\", \"percent\":%.1f, \"attended\":%d, \"total\":%d}", atts[i].courseCode, percent, atts[i].attended, atts[i].total);
            strcat(output, buf);
        }
    }
    strcat(output, "]"); return output;
}

EMSCRIPTEN_KEEPALIVE
const char* api_teacher_mark_attendance(const char* teacherId, const char* courseCode, const char* studentId, const char* isPresentStr) {
    int ti = findTeacher(teacherId); int si = findStudent(studentId); int c = findCourse(courseCode);
    if (ti < 0 || si < 0 || c < 0) return "Error: Invalid IDs.";
    if (strcmp(courses[c].teacherId, teachers[ti].id) != 0) return "Error: Not your course.";
    int isPresent = atoi(isPresentStr);
    int ai = findAtt(studentId, courseCode);
    if (ai >= 0) {
        atts[ai].total += 1; if (isPresent) atts[ai].attended += 1;
    } else {
        if (attCount >= MAX_ATT) return "Error: Storage full.";
        copyStr(atts[attCount].studentId, 16, studentId); copyStr(atts[attCount].courseCode, 12, courseCode);
        atts[attCount].total = 1; atts[attCount].attended = isPresent ? 1 : 0; attCount++;
    }
    saveAtts(); return "Success: Attendance marked.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_add_student(const char* id, const char* pass, const char* name, const char* prog, const char* semStr) {
    if (findStudent(id) >= 0) return "Error: ID already exists.";
    Student s; copyStr(s.id, 16, id); copyStr(s.pass, 24, pass); copyStr(s.name, 50, name); copyStr(s.program, 30, prog);
    s.semester = atoi(semStr); s.active = 1; students[studentCount++] = s; saveStudents(); return "Success: Student added.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_update_student(const char* id, const char* name, const char* prog, const char* semStr, const char* pass) {
    int si = findStudent(id); if (si < 0) return "Error: Student not found.";
    if (name[0]) copyStr(students[si].name, 50, name);
    if (prog[0]) copyStr(students[si].program, 30, prog);
    if (semStr[0]) students[si].semester = atoi(semStr);
    if (pass[0]) copyStr(students[si].pass, 24, pass);
    students[si].active = 1; saveStudents(); return "Success: Student updated.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_delete_student(const char* id) {
    int si = findStudent(id); if (si < 0) return "Error: Student not found.";
    for (int i = si; i < studentCount - 1; i++) { students[i] = students[i + 1]; }
    studentCount--; 
    int newMarkCount = 0; for (int i = 0; i < markCount; i++) { if (strcmp(marks[i].studentId, id) != 0) { marks[newMarkCount++] = marks[i]; } }
    markCount = newMarkCount;
    int newAttCount = 0; for (int i = 0; i < attCount; i++) { if (strcmp(atts[i].studentId, id) != 0) { atts[newAttCount++] = atts[i]; } }
    attCount = newAttCount;
    saveStudents(); saveMarks(); saveAtts(); return "Success: Student completely deleted.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_list_students() {
    static char output[4096]; output[0] = '\0'; char buf[300];
    strcat(output, "<table><tr><th>ID</th><th>Name</th><th>Program</th><th>Sem</th><th>Status</th></tr>");
    for (int i = 0; i < studentCount; i++) {
        sprintf(buf, "<tr><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>%s</td></tr>", students[i].id, students[i].name, students[i].program, students[i].semester, students[i].active ? "Active" : "Inactive");
        strcat(output, buf);
    }
    strcat(output, "</table>"); return output;
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_add_teacher(const char* id, const char* pass, const char* name, const char* dept) {
    if (findTeacher(id) >= 0) return "Error: Teacher ID exists.";
    Teacher t; copyStr(t.id, 16, id); copyStr(t.pass, 24, pass); copyStr(t.name, 50, name); copyStr(t.dept, 30, dept);
    teachers[teacherCount++] = t; saveTeachers(); return "Success: Teacher added.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_get_teachers() {
    static char output[4096]; output[0] = '\0'; char buf[200];
    for (int i = 0; i < teacherCount; i++) { sprintf(buf, "%s|%s\n", teachers[i].id, teachers[i].name); strcat(output, buf); }
    return output;
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_add_course(const char* code, const char* title, const char* creditsStr, const char* teacherId) {
    if (findCourse(code) >= 0) return "Error: Course code exists.";
    if (findTeacher(teacherId) < 0) return "Error: Teacher not found.";
    Course c; copyStr(c.code, 12, code); copyStr(c.title, 40, title); c.credits = atoi(creditsStr); copyStr(c.teacherId, 16, teacherId);
    courses[courseCount++] = c; saveCourses(); return "Success: Course added.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_admin_list_courses() {
    static char output[4096]; output[0] = '\0'; char buf[300];
    strcat(output, "<table><tr><th>Code</th><th>Title</th><th>Credits</th><th>Teacher</th></tr>");
    for (int i = 0; i < courseCount; i++) {
        int ti = findTeacher(courses[i].teacherId);
        sprintf(buf, "<tr><td>%s</td><td>%s</td><td>%d</td><td>%s</td></tr>", courses[i].code, courses[i].title, courses[i].credits, ti >= 0 ? teachers[ti].name : courses[i].teacherId);
        strcat(output, buf);
    }
    strcat(output, "</table>"); return output;
}

EMSCRIPTEN_KEEPALIVE
const char* api_teacher_view_student(const char* teacherId, const char* studentId) {
    static char output[4096]; output[0] = '\0';
    int ti = findTeacher(teacherId); int si = findStudent(studentId);
    if (ti < 0 || si < 0) return "<div class='error'>Invalid IDs.</div>";
    int hasAccess = 0; for (int i = 0; i < markCount; i++) { if (strcmp(marks[i].studentId, studentId) == 0) { int c = findCourse(marks[i].courseCode); if (c >= 0 && strcmp(courses[c].teacherId, teachers[ti].id) == 0) { hasAccess = 1; break; } } }
    if (!hasAccess) return "<div class='error'>ACCESS DENIED: Student not in your courses.</div>";
    char buf[300];
    sprintf(output, "<h3>Result: %s</h3>", students[si].name);
    strcat(output, "<table><tr><th>Code</th><th>Course</th><th>Marks</th></tr>");
    for (int i = 0; i < markCount; i++) {
        if (strcmp(marks[i].studentId, studentId) == 0) {
            int c = findCourse(marks[i].courseCode);
            if (c >= 0 && strcmp(courses[c].teacherId, teachers[ti].id) == 0) {
                sprintf(buf, "<tr><td>%s</td><td>%s</td><td>%.1f</td></tr>", courses[c].code, courses[c].title, marks[i].marks);
                strcat(output, buf);
            }
        }
    }
    strcat(output, "</table>"); return output;
}

EMSCRIPTEN_KEEPALIVE
const char* api_teacher_enter_marks(const char* teacherId, const char* courseCode, const char* studentId, const char* marksStr) {
    int ti = findTeacher(teacherId); int si = findStudent(studentId); int c = findCourse(courseCode);
    if (ti < 0 || si < 0 || c < 0) return "Error: Invalid IDs.";
    if (strcmp(courses[c].teacherId, teachers[ti].id) != 0) return "Error: Not your course.";
    float m = atof(marksStr); if (m < 0 || m > 100) return "Error: Marks 0-100.";
    int mi = findMark(studentId, courseCode);
    if (mi >= 0) { marks[mi].marks = m; } else { copyStr(marks[markCount].studentId, 16, studentId); copyStr(marks[markCount].courseCode, 12, courseCode); marks[markCount].marks = m; markCount++; }
    saveMarks(); return "Success: Marks saved.";
}

EMSCRIPTEN_KEEPALIVE
const char* api_teacher_list_roster(const char* teacherId, const char* courseCode) {
    static char output[4096]; output[0] = '\0';
    int ti = findTeacher(teacherId); int c = findCourse(courseCode);
    if (ti < 0 || c < 0 || strcmp(courses[c].teacherId, teachers[ti].id) != 0) return "Error: Invalid course.";
    char buf[300]; int found = 0;
    sprintf(output, "<h3>Roster: %s</h3>", courses[c].title);
    strcat(output, "<table><tr><th>ID</th><th>Name</th><th>Marks</th></tr>");
    for (int i = 0; i < markCount; i++) {
        if (strcmp(marks[i].courseCode, courseCode) == 0) {
            int si = findStudent(marks[i].studentId); if (si >= 0) {
                sprintf(buf, "<tr><td>%s</td><td>%s</td><td>%.1f</td></tr>", students[si].id, students[si].name, marks[i].marks);
                strcat(output, buf); found = 1;
            }
        }
    }
    if (!found) strcat(output, "<tr><td colspan='3'>No marks yet.</td></tr>");
    strcat(output, "</table>"); return output;
}

/* =========================================================
   EXPORT & IMPORT FUNCTIONS (For Team Data Merging)
   ========================================================= */

EMSCRIPTEN_KEEPALIVE
const char* api_export_db() {
    static char output[8192]; output[0] = '\0'; char buf[200];
    for (int i = 0; i < studentCount; i++) { sprintf(buf, "S|%s|%s|%s|%s|%d|%d\n", students[i].id, students[i].pass, students[i].name, students[i].program, students[i].semester, students[i].active); strcat(output, buf); }
    for (int i = 0; i < teacherCount; i++) { sprintf(buf, "T|%s|%s|%s|%s\n", teachers[i].id, teachers[i].pass, teachers[i].name, teachers[i].dept); strcat(output, buf); }
    for (int i = 0; i < courseCount; i++) { sprintf(buf, "C|%s|%s|%d|%s\n", courses[i].code, courses[i].title, courses[i].credits, courses[i].teacherId); strcat(output, buf); }
    for (int i = 0; i < markCount; i++) { sprintf(buf, "M|%s|%s|%.1f\n", marks[i].studentId, marks[i].courseCode, marks[i].marks); strcat(output, buf); }
    for (int i = 0; i < attCount; i++) { sprintf(buf, "A|%s|%s|%d|%d\n", atts[i].studentId, atts[i].courseCode, atts[i].total, atts[i].attended); strcat(output, buf); }
    return output;
}

EMSCRIPTEN_KEEPALIVE
int api_import_db(const char* data) {
    char *copy = strdup(data);
    char *line = strtok(copy, "\n");
    
    while (line != NULL) {
        if (line[0] == 'S') {
            char *t = strtok(line, "|"); t = strtok(NULL, "|"); char temp_id[16]; copyStr(temp_id, 16, t);
            if (findStudent(temp_id) < 0) {
                Student s; copyStr(s.id, 16, temp_id);
                t = strtok(NULL, "|"); copyStr(s.pass, 24, t); t = strtok(NULL, "|"); copyStr(s.name, 50, t);
                t = strtok(NULL, "|"); copyStr(s.program, 30, t); t = strtok(NULL, "|"); s.semester = atoi(t);
                t = strtok(NULL, "|"); s.active = atoi(t); students[studentCount++] = s;
            }
        } else if (line[0] == 'T') {
            char *t = strtok(line, "|"); t = strtok(NULL, "|"); char temp_id[16]; copyStr(temp_id, 16, t);
            if (findTeacher(temp_id) < 0) {
                Teacher x; copyStr(x.id, 16, temp_id);
                t = strtok(NULL, "|"); copyStr(x.pass, 24, t); t = strtok(NULL, "|"); copyStr(x.name, 50, t);
                t = strtok(NULL, "|"); copyStr(x.dept, 30, t); teachers[teacherCount++] = x;
            }
        } else if (line[0] == 'C') {
            char *t = strtok(line, "|"); t = strtok(NULL, "|"); char temp_code[12]; copyStr(temp_code, 12, t);
            if (findCourse(temp_code) < 0) {
                Course c; copyStr(c.code, 12, temp_code);
                t = strtok(NULL, "|"); copyStr(c.title, 40, t); t = strtok(NULL, "|"); c.credits = atoi(t);
                t = strtok(NULL, "|"); copyStr(c.teacherId, 16, t); courses[courseCount++] = c;
            }
        } else if (line[0] == 'M') {
            char *t = strtok(line, "|"); t = strtok(NULL, "|"); char temp_sid[16]; copyStr(temp_sid, 16, t);
            t = strtok(NULL, "|"); char temp_ccode[12]; copyStr(temp_ccode, 12, t);
            if (findMark(temp_sid, temp_ccode) < 0) {
                Mark m; copyStr(m.studentId, 16, temp_sid); copyStr(m.courseCode, 12, temp_ccode);
                t = strtok(NULL, "|"); m.marks = atof(t); marks[markCount++] = m;
            }
        } else if (line[0] == 'A') {
            char *t = strtok(line, "|"); t = strtok(NULL, "|"); char temp_sid[16]; copyStr(temp_sid, 16, t);
            t = strtok(NULL, "|"); char temp_ccode[12]; copyStr(temp_ccode, 12, t);
            if (findAtt(temp_sid, temp_ccode) < 0) {
                Attendance a; copyStr(a.studentId, 16, temp_sid); copyStr(a.courseCode, 12, temp_ccode);
                t = strtok(NULL, "|"); a.total = atoi(t); t = strtok(NULL, "|"); a.attended = atoi(t); atts[attCount++] = a;
            }
        }
        line = strtok(NULL, "\n");
    }
    
    saveStudents(); saveTeachers(); saveCourses(); saveMarks(); saveAtts();
    free(copy);
    return 1;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    return 0;
}