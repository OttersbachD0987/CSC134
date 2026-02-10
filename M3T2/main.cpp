// CSC 134
// M3T1
// Daley Ottersbach
// 1/28/2026

#include <iostream>
#include <iomanip>
#include <random>

int main(int argc, char** argv) {
    std::random_device random = std::random_device();
    std::mt19937 generator = std::mt19937(random());
    std::uniform_int_distribution<uint32_t> dieSixDistrubtion = std::uniform_int_distribution<uint32_t>(1, 6);
    
    //dieSixDistrubtion(generator) + dieSixDistrubtion(generator);

    std::cout << "CRAPS" << std::endl;

    bool win = false;
    bool running = true;
    int32_t roll = dieSixDistrubtion(generator) + dieSixDistrubtion(generator);
    int32_t points = roll;
    std::cout << "You rolled a " << roll << "." << std::endl;
    switch (roll) {
        case 7:
        case 11:
            win = true;
            goto postGame;
    }
    std::cout << "Your target points are " << points << std::endl;
    while (running) {
        roll = dieSixDistrubtion(generator) + dieSixDistrubtion(generator);
        std::cout << "You rolled a " << roll << ".";
        std::cin.get();
        if (roll == points) {
            win = true;
            running = false;
            break;
        } else if (roll == 7) {
            running = false;
            break;
        }
    }
    postGame:
    if (win) {
        std::cout << "You won." << std::endl;
    } else {
        std::cout << "You rolled a 7, that's a loss." << std::endl;
    }

    return 0;
}