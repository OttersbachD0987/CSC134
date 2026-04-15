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

int main(int argc, char** argv) {
    /*
    std::random_device random = std::random_device();
    std::mt19937 generator = std::mt19937(random());
    std::uniform_int_distribution<uint32_t> digitDistribution = std::uniform_int_distribution<uint32_t>(1, 9);
    */

    std::cout << std::setprecision(2) << std::fixed;

    std::cout << "Q1" << std::endl;
    {
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

    std::cout << "Q2" << std::endl;
    {
        float length;
        float width;
        float height;
        BoundedInput<float>("Length of block: ", length, 0, std::numeric_limits<float>::max());
        BoundedInput<float>("Width of block: ", width, 0, std::numeric_limits<float>::max());
        BoundedInput<float>("Height of block: ", height, 0, std::numeric_limits<float>::max());
        std::cout << "The volume of the block is " << (length * width * height) << "." << std::endl;
    }

    std::cout << "Q3" << std::endl;
    {
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

    std::cout << "Q4" << std::endl;
    {
    }

    std::cout << "Q5" << std::endl;
    {
    }

    std::cout << "Q6" << std::endl;
    {
    }
    return 0;
}