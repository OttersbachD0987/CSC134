// CSC 134
// M6T1
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <string>
#include <cstdint>

const size_t SIZE = 5;
const size_t SCALE = 1;

int main() {
    int cars[SIZE];
    float sum = 0;
    float average;

    std::cout << "This program will ask you to enter a count of cars seen." << std::endl;

    for (size_t i = 0; i < SIZE; i++) {
        std::cout << "Day " << i + 1 << ": ";
        std::cin >> cars[i];
        sum += cars[i];
    }

    average = sum / SIZE;
    std::cout << "Over " << SIZE << " days," << std::endl;
    std::cout << "Total cars: " << sum << std::endl;
    std::cout << "Average:    " << average << std::endl;

    for (size_t i = 0; i < SIZE; i++) {
        std::cout << "DAY " << (i + 1) << " | ";
        for (size_t b = 0; b < cars[i] / SCALE; b++) {
            std::cout << "█";
        }
        std::cout << " " << cars[i] << "\n";
    }
}