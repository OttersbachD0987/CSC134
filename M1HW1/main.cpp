// CSC 134
// M1HW1
// Daley Ottersbach
// 1/12/2026

#include <iostream>
#include <iomanip>
#include <stdint.h>

const std::string NAME = "Jane Smith";
const int32_t DATE = 100;
const float GROSS = 0.25f;

int main(int argc, char** argv) {
    std::cout << "The movie " << NAME << " is ." << std::endl;
    std::cout << NAME << " came out in " << DATE << "." << std::endl;
    std::cout << NAME << " had a worldwide gross of $" << std::setprecision(2) << GROSS << "." << std::endl;
    std::cout << "In the movie the character _ said \"\"." << std::endl;
    return 0;
}