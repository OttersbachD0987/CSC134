// CSC 134
// M2HW1 - Gold
// Daley Ottersbach
// 2/11/2026

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cmath>

// Implementation of a hashing function I did a while back. Don't remember what the hashing function is called.
// https://github.com/Darthsae/CStandardLibrary/blob/4335c35da18fb7b0e3bf992bf6240e4b30acc99e/src/CString.c#L48
uint64_t HashString(std::string_view a_string) {
    const char* str = a_string.data();
    uint64_t hash = 5381;
    char c = 0;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// Made this near begining of class, it was previously in the Util folder in Input.cpp
template <typename T>
void Input(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    std::cin >> a_value;
}

const float CRATE_PRICE_PER_CUBIC_METER = 0.3f;
const float CRATE_CHARGE_PER_CUBIC_METER = 0.52f;
const uint32_t SLICES_PER_VISITOR = 3;

int main(int argc, char** argv) {
    std::cout << std::setprecision(2) << std::fixed;

    std::cout << "Q1" << std::endl;
    {
        char* userName = (char*)malloc(255);
        float startingBalance;
        float depositAmount;
        float withdrawalAmount;
        std::cout << "Your name: ";
        std::cin.getline(userName, 255, '\n');
        Input<float>("Your starting balance: ", startingBalance);
        Input<float>("Your deposit amount: ", depositAmount);
        Input<float>("Your withdrawal amount: ", withdrawalAmount);
        uint64_t userHash = HashString(userName);
        float finalBalance = startingBalance + depositAmount - withdrawalAmount;

        std::cout << "Account "<< " #" << userHash << " ("  << userName << ")\nFinal Balance: $" << finalBalance << std::endl;
    }

    std::cout << "Q2" << std::endl;
    {
        float crateLength;
        float crateWidth;
        float crateHeight;
        Input<float>("Crate Length: ", crateLength);
        Input<float>("Crate Width: ", crateWidth);
        Input<float>("Crate Height: ", crateHeight);
        float crateVolume = crateLength * crateWidth * crateHeight;
        float crateCost = crateVolume * CRATE_PRICE_PER_CUBIC_METER;
        float customersCharge = crateVolume * CRATE_CHARGE_PER_CUBIC_METER;
        float crateProfit = customersCharge - crateCost;

        std::cout << "Crate Volume: " << crateVolume << "m^3." << std::endl;
        std::cout << "Crate Cost: $" << crateCost << "." << std::endl;
        std::cout << "Customer's Charge: $" << customersCharge << "." << std::endl;
        std::cout << "Profit: $" << crateProfit << "." << std::endl;
    }

    std::cout << "Q3" << std::endl;
    {
        uint32_t numberOfPizzas;
        uint32_t slicesPerPizza;
        uint32_t numberOfVisitors;
        Input<uint32_t>("Number of Pizzas: ", numberOfPizzas);
        Input<uint32_t>("Slices per Pizza: ", slicesPerPizza);
        Input<uint32_t>("Number of Visitors: ", numberOfVisitors);
        int32_t leftoverSlices = numberOfPizzas * slicesPerPizza - numberOfVisitors * SLICES_PER_VISITOR;

        if (leftoverSlices > 0) {
            std::cout << "There are " << leftoverSlices << " slices of pizza left over." << std::endl;
        } else if (leftoverSlices < 0) {
            std::cout << "There are not enough slices, you need " << -leftoverSlices << " more slices, or " << (uint32_t)(std::ceil((float)(-leftoverSlices) / slicesPerPizza)) << " more pizzas." << std::endl;
        } else {
            std::cout << "There are no slices of pizza left over." << std::endl;
        }
    }

    std::cout << "Q4" << std::endl;
    {
        std::string letsGo = "Let's go ";
        std::string school = "FTCC";
        std::string team = "Trojans";
        std::string cheerOne = letsGo + school;
        std::string cheerTwo = letsGo + team;

        for (uint32_t i = 0; i < 3; ++i) {
            std::cout << cheerOne << std::endl;
        }
        std::cout << cheerTwo << std::endl;
    }
    return 0;
}