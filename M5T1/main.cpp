// CSC 134
// M5T1
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <string>

void PrintAnswer(const std::string& a_message) {
    std::cout << a_message << std::endl;
}

std::string FormatAnswer(int a_answer) {
    return "The answer is " + std::to_string(a_answer);
}

int main(int argc, char** argv) {
    PrintAnswer(FormatAnswer(5));
    return 0;
}
