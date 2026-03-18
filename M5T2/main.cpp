// CSC 134
// M5T2
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <string>
#include <cstdint>

size_t Square(size_t a_num) {
    return a_num * a_num;
}

void PrintLine(const std::string& a_message) {
    std::cout << a_message << std::endl;
}

int main(int argc, char** argv) {
    for (size_t i = 1; i < 11; ++i) {
        PrintLine(std::to_string(i) + " squared is " + std::to_string(Square(i)));
    }
    return 0;
}
