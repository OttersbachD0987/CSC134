#include <iostream>
#include <cstdint>
#include <limits>

void BoundedInput(size_t& a_value, size_t a_min, size_t a_max) {
    while (!(std::cin >> a_value) || a_value < a_min || a_value > a_max) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char** argv) {
    size_t number;
    std::cout << "Number between 1 and 12: ";
    BoundedInput(number, 1, 12);
    for (size_t i = 1; i <= 12; ++i) {
        std::cout << number << " times " << i << " is " << number * i << "." << std::endl;
    }
    std::cout << std::flush;
    return 0;
}