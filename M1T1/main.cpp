// CSC 134
// M1T1
// Daley Ottersbach
// 1/14/2026

#include <iostream>

const char TEXT[] = "Hello world!";

int main(int argc, char** argv) {
    std::cout << TEXT << std::endl;
    std::cout << "This isn't a variable." << std::endl;
    std::cout << "The line two lines above this one is a variable." << std::endl;

    if (argc > 1) {
        std::cout << "There are " << (argc - 1) << " arguments." << std::endl;
        for (int i = 1; i < argc; ++i) {
            std::cout << "Arg " << i << ": " << argv[i] << "." << std::endl;
        }
    } else {
        std::cout << "No arguments provided." << std::endl;
    }

    std::string userInput;
    std::cout << "Input your name here: ";
    std::cin >> userInput;
    std::cout << "Hello " << userInput << "." << std::endl;
    
    return 0;
}