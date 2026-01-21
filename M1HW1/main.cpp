// CSC 134
// M1HW1
// Daley Ottersbach
// 1/12/2026

#include <iostream>
#include <iomanip>
#include <stdint.h>

const std::string MERCHANT_NAME = "Jane Smith";
const int32_t DATE = 100;
const float GROSS = 0.25f;

int main(int argc, char** argv) {
    std::cout << "The movie " << MERCHANT_NAME << " is ." << std::endl;
    std::cout << MERCHANT_NAME << " came out in " << DATE << "." << std::endl;
    std::cout << MERCHANT_NAME << " had a worldwide gross of $" << std::setprecision(2) << std::fixed << GROSS << "." << std::endl;
    std::cout << "In the movie the character _ said \"\"." << std::endl;
    return 0;
}