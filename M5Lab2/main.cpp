// CSC 134
// M5LAB2
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

void SafeInput(float& a_value) {
    while (!(std::cin >> a_value) || a_value <= 0) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

float getLength(void) {
    float length;
    std::cout << "Length of Rectangle: ";
    SafeInput(length);
    return length;
}

float getWidth(void) {
    float width;
    std::cout << "Width of Rectangle: ";
    SafeInput(width);
    return width;
}

float getArea(float a_length, float a_width) {
    return a_length * a_width;
}

void displayData(float a_length, float a_width, float a_area) {
    std::cout << "Length: " << a_length << "\nWidth: " << a_width << "\nArea: " << a_area << std::endl;
}

int main(int argc, char** argv) {
    float length = getLength();
    float width = getWidth();
    float area = getArea(length, width);

    displayData(length, width, area);

    return 0;
}
