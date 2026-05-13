#ifndef RECTANGLE_H
#define RECTANGLE_H
#include <cstdint>

struct Rectangle {
public:
    uint32_t width;
    uint32_t height;

    uint32_t GetArea(void) {
        return width * height;
    }

    void Draw(void) {
        for (size_t i = 0; i < height; ++i) {
            for (size_t u = 0; u < width; ++u) {
                std::cout << "#";
            }
            std::cout << std::endl;
        }
    }
};
#endif