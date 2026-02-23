// CSC 134
// M3HW1 - Gold
// Daley Ottersbach
// 2/23/2026

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cmath>
#include <random>

// Made this near begining of class, it was previously in the Util folder in Input.cpp
template <typename T>
void Input(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    std::cin >> a_value;
}

template <typename T>
void SafeInput(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(10000, '\n'); 
    }
}


const float SALES_TAX_PERCENT = 0.08f;
const float TIP_PERCENT = 0.15f;

int main(int argc, char** argv) {
    std::random_device random = std::random_device();
    std::mt19937 generator = std::mt19937(random());
    std::uniform_int_distribution<uint32_t> digitDistribution = std::uniform_int_distribution<uint32_t>(1, 9);

    std::cout << std::setprecision(2) << std::fixed;

    std::cout << "Q1" << std::endl;
    {
        std::string response;
        std::cout << "Hello, I'm a c++ program!" << std::endl;
        SafeInput<std::string>("Do you like me? Please type yes or no: ", response);

        if (response == "yes") {
            std::cout << "That's great! I'm sure we'll get along." << std::endl;
        } else if (response == "no") {
            std::cout << "Well, maybe you'll learn to like me later." << std::endl;
        } else {
            std::cout << "If you're not sure... That's ok." << std::endl;
        }
    }

    std::cout << "Q2" << std::endl;
    {
        float price;
        uint32_t choice;
        SafeInput<float>("Enter the price of the meal: ", price);
        SafeInput<uint32_t>("Enter 1 if the meal is dine in, 2 if it is to go: ", choice);
        std::cout << "RECEIPT" << std::endl;
        float salesTax = price * SALES_TAX_PERCENT;
        float finalCost = price + salesTax;
        std::cout << "Product Price: $" << price << "." << std::endl;
        std::cout << (SALES_TAX_PERCENT * 100) << "\% Sales Tax: $" << salesTax << "." << std::endl;
        if (choice == 1) {
            float tip = price * TIP_PERCENT;
            finalCost += tip;
            std::cout << (TIP_PERCENT * 100) << "\% Tip: $" << tip << "." << std::endl;
        }
        std::cout << "Final Cost: $" << finalCost << "." << std::endl;
    }

    std::cout << "Q3" << std::endl;
    {
        uint32_t choice;
        std::cout << "Choose An Arbitrarily Arbitrary Adventure Game (CAAAAG)\n\nYou stand before a set of two doors, the one on the left is made of alabaster, and the one on the right is made of marble.\n\n1) Go through the right door.\n2) Go through the left door.\n" << std::endl;
        do {
            SafeInput<uint32_t>("Choice: ", choice);
        } while (choice != 1 && choice != 2);
        switch (choice) {
            case 1:
                std::cout << "Choose An Arbitrarily Arbitrary Adventure Game (CAAAAG)\n\nAs you step through the door of marble, you hear a clicking sound, and are instantly turned into a pile of dust.\n\n\n- \"Complaints have been filed that such a mechanic is too punishing, however, if Gary Gygax can write THAT adventure, then so can we.\"" << std::endl;
                break;
            case 2:
                std::cout << "Choose An Arbitrarily Arbitrary Adventure Game (CAAAAG)\n\nYou move through the doors of alabaster, and for a moment, it appears as though there were no doors at all. Before you now are two skulls, one of blue, one of red.\n\n1) Pick up the blue skull.\n2) Pick up the red skull.\n" << std::endl;
                do {
                    SafeInput<uint32_t>("Choice: ", choice);
                } while (choice != 1 && choice != 2);
                switch (choice) {
                    case 1:
                        std::cout << "Choose An Arbitrarily Arbitrary Adventure Game (CAAAAG)\n\nYou pick up the blue skull, and all the colors in the world mean naught. Yet still do you languish here in dreams eternal.\n\n\n- \"This is the bad ending. Unlike Clair Obscura Expedition 33, we aren't afraid to label our bad ending a bad ending.\"" << std::endl;
                        break;
                    case 2:
                        std::cout << "Choose An Arbitrarily Arbitrary Adventure Game (CAAAAG)\n\nThe red skull melts into a puddle of sanguine slime, the strong smell of petrichor fills the room. You wake up, freed from the dream.\n\n\n- \"This is the good ending. You're very good at this. Did you just pick the second option by default?\"" << std::endl;
                        break;
                }
                break;
        }
    }

    std::cout << "Q4" << std::endl;
    {
        uint32_t numOne = digitDistribution(generator);
        uint32_t numTwo = digitDistribution(generator);
        uint32_t userChoice;
        std::cout << "What is " << numOne << " + " << numTwo << "?" << std::endl;
        SafeInput<uint32_t>("Answer: ", userChoice);
        if (userChoice == numOne + numTwo) {
            std::cout << "Correct." << std::endl;
        } else {
            std::cout << "Incorrect." << std::endl;
        }
    }
    return 0;
}