// CSC 134
// M1LAB
// Daley Ottersbach
// 1/21/2026

#include <iostream>
#include <iomanip>
#include <stdint.h>

const std::string ITEM_NAME = "apples";
const std::string MERCHANT_NAME = "Jane Smith";
const int32_t ITEM_COUNT = 125;
const float PRICE_PER_ITEM = 0.20f;

int main(int argc, char** argv) {
    float totalCost = ITEM_COUNT * PRICE_PER_ITEM;

    std::cout << "Welcome to " << MERCHANT_NAME << "'s " << ITEM_NAME << " store." << std::endl;
    std::cout << "We have " << ITEM_COUNT << " " << ITEM_NAME << " in stock." << std::endl;
    std::cout << ITEM_NAME << " are currently $" << std::setprecision(2) << std::fixed << PRICE_PER_ITEM << " each." << std::endl;
    std::cout << "If you want them all, that will be $" << std::setprecision(2) << std::fixed << totalCost << "." << std::endl;
    return 0;
}