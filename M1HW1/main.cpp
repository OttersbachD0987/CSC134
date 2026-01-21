// CSC 134
// M1HW1
// Daley Ottersbach
// 1/21/2026

#include <iostream>
#include <iomanip>
#include <stdint.h>

const std::string MOVIE_NAME = "Movie";
const int32_t DATE = 1900;
const float GROSS = 0.0f;

int main(int argc, char** argv) {
    std::cout << std::setprecision(2) << std::fixed;
    std::cout << "The movie is " << MOVIE_NAME << "." << std::endl;
    std::cout << MOVIE_NAME << " came out in " << DATE << "." << std::endl;
    std::cout << MOVIE_NAME << " had a worldwide gross of $" << GROSS << " million." << std::endl;
    std::cout << "In the movie the character _ said \"\"." << std::endl;
    return 0;
}