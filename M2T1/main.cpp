// CSC 134
// M2T1
// Daley Ottersbach
// 1/12/2026

#include <iostream>
#include <iomanip>
#include <stdint.h>





int main() {
    std::string name;
    int32_t apples;
    float pricePerApple;

    std::cout << "Enter your name: ";
    std::cin >> name;

    std::cout << "Enter the number of apples: ";
    std::cin >> apples;

    std::cout << "Enter the price per apple: ";
    std::cin >> pricePerApple;

    float totalCost = apples * pricePerApple;

    std::cout << "Welcome to " << name << "'s apple orchard." << std::endl;
    std::cout << "We have " << apples << " apples in stock." << std::endl;
    std::cout << "Apples are currently $" << std::setprecision(2) << pricePerApple << " each." << std::endl;
    std::cout << "If you want them all, that will be $" << totalCost << "." << std::endl;
    return 0;
}