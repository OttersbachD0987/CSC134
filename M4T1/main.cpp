#include <iostream>
#include <cstdint>

const int32_t MIN_NUM = 1;
const int32_t MAX_NUM = 10;

int main(int argc, char** argv) {
    int32_t num = 0;
    while (num < 5) {
        std::cout << "Hello\n";
        ++num;
    }
    std::cout << "That's all!\nNumber Number Squared\n-------------------------" << std::endl;
    num = MIN_NUM;
    while (num <= MAX_NUM) {
        std::cout << num << "\t\t" << (num * num) << std::endl;
        ++num;
    }
    return 0;
}