// CSC 134
// M6LAB1
// Daley Ottersbach
// 4/20/2026

/// IMPORTANT
// sudo apt update
// sudo apt install ffmpeg

#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <corecrt_math_defines.h>

void SingleFrame(std::string a_name, int32_t a_bar) {
    std::ofstream file = std::ofstream(a_name, std::ios::binary);
    uint32_t width = 1920;
    uint32_t height = 1080;
    float size = sqrtf(width * width + height * height);
    uint8_t r, g, b;
    float ux = 255.0f * (0.5f + 0.5f * cosf(a_bar * M_PI / 255)) / (float)width;
    float uy = 255.0f * (0.5f + 0.5f * sinf(a_bar * M_PI / 255)) / (float)height;
    file << std::format("P6\n{} {}\n255\n", width, height);
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            r = (uint8_t)(a_bar);
            g = (uint8_t)((float)y * uy);
            b = (uint8_t)((float)x * ux);
            file << r << g << b;
        }
    }
    file << std::flush;
}

int main(int argc, char** argv) {    
    std::ofstream file = std::ofstream("a.ppm", std::ios::binary);
    uint32_t width = 100;
    uint32_t height = 100;
    float size = sqrtf(width * width + height * height);
    uint8_t r, g, b;
    file << std::format("P6\n{} {}\n255\n", width, height);
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            r = (uint8_t)(255.0f * sqrtf((y - 50) * (y - 50) + (x - 50) * (x - 50)) / size);
            g = (uint8_t)(255.0f * (float)y / (float)height);
            b = (uint8_t)(255.0f * (float)x / (float)width);
            file << r << g << b;
        }
    }
    file << std::flush;
    for (int i = 0; i < 256; ++i) {
        std::cout << i << std::endl;
        SingleFrame(std::format("Grifty/Image{}.ppm", i), i);
    }
    for (int i = 0; i < 256; ++i) {
        std::cout << (255 - i) << std::endl;
        SingleFrame(std::format("Grifty/Image{}.ppm", 256 + i), 255 - i);
    }
    system("ffmpeg -y -f image2 -framerate 60 -i Grifty/Image%d.ppm -q:v 12 video.mp4");
}