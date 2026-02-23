// CSC 134
// M3LAB2
// Daley Ottersbach
// 2/23/2026

#include <iostream>
#include <cstdint>

int main(int argc, char** argv) {
    float numericGrade;
    std::cout << "Grade (numeric): ";
    std::cin >> numericGrade;

    if (numericGrade >= 90) {
        std::cout << "\nGrade: A" << std::endl;
    } else if (numericGrade >= 80) {
        std::cout << "\nGrade: B" << std::endl;
    } else if (numericGrade >= 70) {
        std::cout << "\nGrade: C" << std::endl;
    } else if (numericGrade >= 60) {
        std::cout << "\nGrade: D" << std::endl;
    } else {
        std::cout << "\nGrade: F" << std::endl;
    }

    return 0;
}