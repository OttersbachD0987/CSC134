// CSC 134
// M3T1
// Daley Ottersbach
// 1/28/2026

#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    float rectangleOneLength;
    float rectangleOneWidth;
    float rectangleTwoLength;
    float rectangleTwoWidth;
    std::cout << "Rectangle One Length: ";
    std::cin >> rectangleOneLength;
    std::cout << "Rectangle One Width: ";
    std::cin >> rectangleOneWidth;
    std::cout << "Rectangle Two Length: ";
    std::cin >> rectangleTwoLength;
    std::cout << "Rectangle Two Width: ";
    std::cin >> rectangleTwoWidth;

    float rectangleOneArea = rectangleOneLength * rectangleOneWidth;
    float rectangleTwoArea = rectangleTwoLength * rectangleTwoWidth;

    std::cout << std::setprecision(2) << std::fixed;
    std::cout << "Rectangle One Area: " << rectangleOneArea << "m^2." << std::endl;
    std::cout << "Rectangle Two Area: " << rectangleTwoArea << "m^2." << std::endl;
    return 0;
}

/*
Ask the user to input the crate’s length.
Ask the user to input the crate’s width.
Ask the user to input the crate’s height.
Calculate the crate’s volume.
Calculate the cost of building the crate.
Calculate the customer’s charge for the crate.
Calculate the profit made from the crate.
Display the crate’s volume.
Display the cost of building the crate.
Display the customer’s charge for the crate.
Display the profit made from the crate.
*/