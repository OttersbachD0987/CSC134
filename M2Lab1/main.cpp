// CSC 134
// M2LAB1
// Daley Ottersbach
// 1/12/2026

#include <iostream>

const float CRATE_PRICE_PER_CUBIC_METER = 0.23f;
const float CRATE_CHARGE_PER_CUBIC_METER = 0.5f;

int main(int argc, char** argv) {
    float crateLength;
    float crateWidth;
    float crateHeight;
    std::cout << "Crate Length: " << std::endl;
    std::cin >> crateLength;
    std::cout << "Crate Width: " << std::endl;
    std::cin >> crateWidth;
    std::cout << "Crate Height: " << std::endl;
    std::cin >> crateHeight;
    float crateVolume = crateLength * crateWidth * crateHeight;
    float crateCost = crateVolume * CRATE_PRICE_PER_CUBIC_METER;
    float customersCharge = crateVolume * CRATE_CHARGE_PER_CUBIC_METER;
    float crateProfit = customersCharge - crateCost;

    std::cout << "Crate Volume: " << crateVolume << "m^3." << std::endl;
    std::cout << "Crate Cost: $" << crateCost << "." << std::endl;
    std::cout << "Customer's Charge: $" << customersCharge << "." << std::endl;
    std::cout << "Profit: $" << crateProfit << "." << std::endl;
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