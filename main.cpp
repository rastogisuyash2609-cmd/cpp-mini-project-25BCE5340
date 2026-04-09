#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <limits>

using namespace std;

// Convert string to uppercase
string toUpperCase(string str) {
    for (char &c : str) {
        c = toupper(c);
    }
    return str;
}

// --- Student Class ---
class Student {
private:
    string regNo;
    string name;
    string attendanceRecord;

public:
    Student(string r = "", string n = "", string rec = "") {
        regNo = toUpperCase(r);
        name = n;
        attendanceRecord = rec;
    }

    string getRegNo() const {
        return regNo;
    }

    string getName() const {
        return name;
    }

    string getRecord() const {
        return attendanceRecord;
    }

    int getTotalClasses() const {
        return attendanceRecord.length();
    }

    int getPresentCount() const {
        int count = 0;
        for (char c : attendanceRecord) {
            if (c == 'P') {
                count++;
            }
        }
        return count;
    }

    void markAttendance(char status) {
        attendanceRecord += status;
    }

    double getAttendancePercentage() const {
        if (attendanceRecord.empty()) {
            return 0.0;
        }

        return (static_cast<double>(getPresentCount()) / attendanceRecord.length()) * 100.0;
    }
};

// --- AttendanceManager Class ---
class AttendanceManager {
private:
    vector<Student> students;
    string filename;

    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            stringstream ss(line);
            string reg, name, record;

            getline(ss, reg, ',');
            getline(ss, name, ',');
            getline(ss, record, ',');

            students.push_back(Student(toUpperCase(reg), name, record));
        }

        file.close();
    }

    void saveToFile() {
        ofstream file(filename);

        for (const auto& s : students) {
            file << s.getRegNo() << "," << s.getName() << "," << s.getRecord() << "\n";
        }

        file.close();
    }

    bool isDuplicate(const string& reg) {
        string inputReg = toUpperCase(reg);

        for (const auto& s : students) {
            if (toUpperCase(s.getRegNo()) == inputReg) {
                return true;
            }
        }
        return false;
    }

public:
    AttendanceManager(string file) {
        filename = file;
        loadFromFile();
    }

    ~AttendanceManager() {
        saveToFile();
    }

    void addStudent(string reg, string name) {
        reg = toUpperCase(reg);

        if (isDuplicate(reg)) {
            cout << "\n[Error] Student with Registration Number " << reg << " already exists!\n";
            return;
        }

        students.push_back(Student(reg, name));
        saveToFile();
        cout << "\n[Success] Student added successfully.\n";
    }

    void markAttendanceForDate() {
        if (students.empty()) {
            cout << "\n[Notice] No students in the system. Add students first.\n";
            return;
        }

        string date;
        cout << "Enter date (DD-MM-YYYY): ";
        cin >> ws;
        getline(cin, date);

        cout << "\n--- Marking Attendance for " << date << " ---\n";

        for (auto& s : students) {
            char status;

            while (true) {
                cout << "Mark attendance for " << s.getRegNo() << " - " << s.getName() << " (P/A): ";
                cin >> status;
                status = toupper(status);

                if (status == 'P' || status == 'A') {
                    s.markAttendance(status);
                    break;
                } else {
                    cout << "[Invalid Input] Please enter 'P' or 'A'.\n";
                }
            }
        }

        saveToFile();
        cout << "[Success] Attendance marked for " << date << ".\n";
    }

    void viewStudentSummary(string reg) {
        reg = toUpperCase(reg);

        for (const auto& s : students) {
            if (toUpperCase(s.getRegNo()) == reg) {
                cout << "\n=== Attendance Summary ===\n";
                cout << "RegNo:      " << s.getRegNo() << "\n";
                cout << "Name:       " << s.getName() << "\n";
                cout << "Record:     " << (s.getRecord().empty() ? "No classes yet" : s.getRecord()) << "\n";
                cout << "Present:    " << s.getPresentCount() << "\n";
                cout << "Total:      " << s.getTotalClasses() << "\n";
                cout << "Percentage: " << fixed << setprecision(2) << s.getAttendancePercentage() << "%\n";
                cout << "==========================\n";
                return;
            }
        }

        cout << "\n[Error] Student not found!\n";
    }

    void generateReports() {
        if (students.empty()) {
            cout << "\n[Notice] No data available to generate reports.\n";
            return;
        }

        double totalClassPercentage = 0.0;

        cout << "\n=== Comprehensive Attendance Report ===\n";
        cout << left << setw(15) << "RegNo" << setw(25) << "Name" << setw(10) << "Percentage" << "\n";
        cout << "------------------------------------------------------\n";

        for (const auto& s : students) {
            double pct = s.getAttendancePercentage();
            totalClassPercentage += pct;

            cout << left << setw(15) << s.getRegNo()
                 << setw(25) << s.getName()
                 << fixed << setprecision(2) << pct << "%\n";
        }

        cout << "\n=== Shortage Predictor (< 75%) ===\n";
        bool shortageFound = false;

        for (const auto& s : students) {
            if (s.getTotalClasses() > 0 && s.getAttendancePercentage() < 75.0) {
                cout << "[Warning] " << s.getRegNo() << " - " << s.getName()
                     << " (" << fixed << setprecision(2) << s.getAttendancePercentage() << "%)\n";
                shortageFound = true;
            }
        }

        if (!shortageFound) {
            cout << "No students currently have an attendance shortage.\n";
        }

        cout << "\n[Class Average Attendance]: "
             << fixed << setprecision(2)
             << (totalClassPercentage / students.size()) << "%\n";
    }

    void predictAttendanceRequirement(string reg) {
        reg = toUpperCase(reg);

        for (const auto& s : students) {
            if (toUpperCase(s.getRegNo()) == reg) {
                int total = s.getTotalClasses();
                int present = s.getPresentCount();

                double currentPct = s.getAttendancePercentage();

                cout << "\n=== Attendance Predictor ===\n";
                cout << "Name: " << s.getName() << "\n";
                cout << "Current %: " << fixed << setprecision(2) << currentPct << "%\n";

                if (total == 0) {
                    cout << "[Info] No attendance marked yet.\n";
                    cout << "If the student attends the next class, attendance will become 100%.\n";
                    cout << "============================\n";
                    return;
                }

                if (currentPct >= 75.0) {
                    cout << "[Good] Already above 75% attendance.\n";
                    cout << "============================\n";
                    return;
                }

                int requiredDays = 0;

                while (true) {
                    total++;
                    present++;
                    requiredDays++;

                    double newPct = (present * 100.0) / total;

                    if (newPct >= 75.0) {
                        break;
                    }
                }

                cout << "[Action Required] You need to attend next "
                     << requiredDays
                     << " classes continuously to reach 75%.\n";
                cout << "============================\n";
                return;
            }
        }

        cout << "\n[Error] Student not found!\n";
    }

    void showTopper() {
        if (students.empty()) {
            cout << "\n[Notice] No students available.\n";
            return;
        }

        double maxPct = -1.0;
        bool found = false;

        for (const auto& s : students) {
            if (s.getTotalClasses() > 0) {
                double pct = s.getAttendancePercentage();
                if (pct > maxPct) {
                    maxPct = pct;
                    found = true;
                }
            }
        }

        if (!found) {
            cout << "\n[Notice] No attendance data available.\n";
            return;
        }

        cout << "\n=== Top Attendance Students ===\n";

        for (const auto& s : students) {
            if (s.getTotalClasses() > 0 && s.getAttendancePercentage() == maxPct) {
                cout << "RegNo:      " << s.getRegNo() << "\n";
                cout << "Name:       " << s.getName() << "\n";
                cout << "Present:    " << s.getPresentCount() << "\n";
                cout << "Total:      " << s.getTotalClasses() << "\n";
                cout << "Percentage: " << fixed << setprecision(2) << s.getAttendancePercentage() << "%\n";
                cout << "------------------------------\n";
            }
        }
    }
};

// --- Main Function ---
int main() {
    AttendanceManager manager("attendance_db.csv");
    int choice;

    do {
        cout << "\n>>> Attendance Manager System <<<\n";
        cout << "1. Add Student\n";
        cout << "2. Mark Attendance for a Particular Date\n";
        cout << "3. View Student Summary\n";
        cout << "4. Generate Class Reports & Shortage List\n";
        cout << "5. Predict Attendance Requirement\n";
        cout << "6. Show Top Attendance Students\n";
        cout << "7. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[Error] Enter only numeric choice.\n";
            continue;
        }

        if (choice == 1) {
            string reg, name;

            cout << "Enter Registration Number: ";
            cin >> ws;
            getline(cin, reg);

            cout << "Enter Full Name: ";
            getline(cin, name);

            manager.addStudent(reg, name);
        }
        else if (choice == 2) {
            manager.markAttendanceForDate();
        }
        else if (choice == 3) {
            string reg;
            cout << "Enter Registration Number to search: ";
            cin >> ws;
            getline(cin, reg);
            manager.viewStudentSummary(reg);
        }
        else if (choice == 4) {
            manager.generateReports();
        }
        else if (choice == 5) {
            string reg;
            cout << "Enter Registration Number: ";
            cin >> ws;
            getline(cin, reg);
            manager.predictAttendanceRequirement(reg);
        }
        else if (choice == 6) {
            manager.showTopper();
        }
        else if (choice == 7) {
            break;
        }
        else {
            cout << "[Error] Invalid choice.\n";
        }

    } while (true);

    cout << "Exiting system. Data has been saved.\n";
    return 0;
}