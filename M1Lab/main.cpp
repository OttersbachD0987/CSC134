// CSC 134
// M1LAB
// Daley Ottersbach
// 1/12/2026

#include <iostream>
#include <iomanip>
#include <stdint.h>

const std::string NAME = "Jane Smith";
const int32_t APPLES = 100;
const float PRICE_PER_APPLE = 0.25f;

int main() {
    float totalCost = APPLES * PRICE_PER_APPLE;

    std::cout << "Welcome to " << NAME << "'s apple orchard." << std::endl;
    std::cout << "We have " << APPLES << " apples in stock." << std::endl;
    std::cout << "Apples are currently $" << std::setprecision(2) << PRICE_PER_APPLE << " each." << std::endl;
    std::cout << "If you want them all, that will be $" << std::setprecision(2) << totalCost << "." << std::endl;
    return 0;
}