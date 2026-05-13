// CSC 134
// M5HW1 - Gold
// Daley Ottersbach
// 3/18/2026

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cmath>
#include <random>
#include <limits>
#include <concepts>

template<std::integral T>
void SafeInput(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

template<std::floating_point T>
void SafeInput(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

template<typename T>
requires(!std::integral<T> && !std::floating_point<T>)
void SafeInput(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
}

template<std::integral T>
void BoundedInput(const char* a_prompt, T& a_value, T a_min, T a_max) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value) || a_value < a_min || a_value > a_max) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

template<std::floating_point T>
void BoundedInput(const char* a_prompt, T& a_value, T a_min, T a_max) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value) || a_value < a_min || a_value > a_max) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

void Rainfall(void) {
    std::string stringHolder;
    float floatHolder;
    std::string months = "";
    float rainfall = 0;
    SafeInput<std::string>("Enter Month: ", stringHolder);
    months += stringHolder;
    BoundedInput<float>(("Rainfall For " + stringHolder + ": ").c_str(), floatHolder, 0, std::numeric_limits<float>::max());
    rainfall += floatHolder;
    SafeInput<std::string>("Enter Month: ", stringHolder);
    months += ", " + stringHolder;
    BoundedInput<float>(("Rainfall For " + stringHolder + ": ").c_str(), floatHolder, 0, std::numeric_limits<float>::max());
    rainfall += floatHolder;
    SafeInput<std::string>("Enter Month: ", stringHolder);
    months += ", and " + stringHolder;
    BoundedInput<float>(("Rainfall For " + stringHolder + ": ").c_str(), floatHolder, 0, std::numeric_limits<float>::max());
    rainfall += floatHolder;
    std::cout << "The average rainfall for " << months << " is " << (rainfall / 3) << " inches." << std::endl;
}

void BlockVolume(void) {
    float length;
    float width;
    float height;
    BoundedInput<float>("Length of block: ", length, 0, std::numeric_limits<float>::max());
    BoundedInput<float>("Width of block: ", width, 0, std::numeric_limits<float>::max());
    BoundedInput<float>("Height of block: ", height, 0, std::numeric_limits<float>::max());
    std::cout << "The volume of the block is " << (length * width * height) << "." << std::endl;
}

void RomanNumerals(void) {
    int32_t number;
    BoundedInput<int32_t>("Number between 1-10: ", number, 1, 10);
    std::cout << "The Roman Numeral of " << number << " is ";
    switch (number) {
        case 1:
            std::cout << "I";
            break;
        case 2:
            std::cout << "II";
            break;
        case 3:
            std::cout << "III";
            break;
        case 4:
            std::cout << "IV";
            break;
        case 5:
            std::cout << "V";
            break;
        case 6:
            std::cout << "VI";
            break;
        case 7:
            std::cout << "VII";
            break;
        case 8:
            std::cout << "VIII";
            break;
        case 9:
            std::cout << "IX";
            break;
        case 10:
            std::cout << "X";
            break;
    }
    std::cout << "." << std::endl;
}

void GeoCalc(void) {
    int32_t option;
    BoundedInput("Geometry Calculator:\n\n1. Area of Circle\n2. Area of Rectangle\n3. Area of Triangle\n4. Quit\n\nChoice: ", option, 1, 4);
    switch (option) {
        case 1:
            int32_t radius;
            BoundedInput("Radius of Circle: ", radius, 1, 10000000);
            std::cout << "The area is " << (3.14159 * radius * radius) << std::endl;
            break;
        case 2:
            int32_t w, h;
            BoundedInput("Width of Rectangle: ", w, 1, 10000000);
            BoundedInput("Height of Rectangle: ", h, 1, 10000000);
            std::cout << "The area is " << (w * h) << std::endl;
            break;
        case 3:
            int32_t b, he;
            BoundedInput("Base of Triangle: ", b, 1, 10000000);
            BoundedInput("Height of Triangle: ", he, 1, 10000000);
            std::cout << "The area is " << (b * he * 0.5) << std::endl;
            break;
    }
}

void Speed(void) {
    int32_t speed;
    int32_t time;
    BoundedInput("Speed in MPH: ", speed, 1, 10000);
    BoundedInput("Hours traveled: ", time, 1, 10000);
    std::cout << "Hour | Distance" << std::endl;
    for (size_t i = 0; i < time; ++i) {
        std::cout << (i + 1) << " | " << (speed * time) << std::endl;
    }
}

int main(int argc, char** argv) {
    /*
    std::random_device random = std::random_device();
    std::mt19937 generator = std::mt19937(random());
    std::uniform_int_distribution<uint32_t> digitDistribution = std::uniform_int_distribution<uint32_t>(1, 9);
    */

    std::cout << std::setprecision(2) << std::fixed;

    uint32_t option;
    while (option != 6) {
        std::cout << "1) Rainfall\n2) Block Volume\n3)Roman Numerals\n4) Geometry Calculator\n5) Speed Calculator\n6) Quit" << std::endl;
        BoundedInput<uint32_t>("Choice: ", option, 1, 6);
        switch (option) {
            case 1:
                Rainfall();
                break;
            case 2:
                BlockVolume();
                break;
            case 3:
                RomanNumerals();
                break;
            case 4:
                GeoCalc();
                break;
            case 5:
                Speed();
                break;
        }
    }
    return 0;
}