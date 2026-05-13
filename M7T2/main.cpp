// CSC 134
// M7T1
// Daley Ottersbach
// 4/27/2026

#include <iostream>
#include <string>
#include <limits>
#include "Rectangle.h"


void BoundedInput(uint32_t& a_value) {
    while (!(std::cin >> a_value) || a_value < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

int main(int argc, char** argv) {
    Rectangle ra = Rectangle{2, 4};
    std::cout << "Rectangle width: ";
    BoundedInput(ra.width);
    std::cout << "Rectangle height: ";
    BoundedInput(ra.height);
    std::cout << "Rectangle area: " << ra.GetArea() << std::endl;
    ra.Draw();
}