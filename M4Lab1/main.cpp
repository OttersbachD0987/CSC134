#include <iostream>
#include <cstdint>

int main(int argc, char** argv) {
    size_t width = 2;
    size_t height = 2;
    for (size_t j = 0; j < height; ++j) {
        for (size_t i = 0; i < width; ++i) {
            std::cout << "*";
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
    return 0;
}