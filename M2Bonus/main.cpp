// CSC 134
// M2BONUS
// Daley Ottersbach
// 1/12/2026

#define _USE_MATH_DEFINES

#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <math.h>

const int32_t RECT_WIDTH = 10;
const int32_t RECT_HEIGHT = 8;
const float RECT_AREA = RECT_HEIGHT * RECT_WIDTH;
const float PRODUCT_PRICE = 99.99f;
const float SALES_TAX_PERCENT = 0.08f;
const float SALES_TAX = PRODUCT_PRICE * SALES_TAX_PERCENT;
const float FINAL_COST = PRODUCT_PRICE + SALES_TAX;
const float PIZZA_DIAMETER = 12;
const float PIZZA_RADIUS = PIZZA_DIAMETER / 2;
const float PIZZA_AREA = M_PI * PIZZA_RADIUS * PIZZA_RADIUS;
const float PIZZA_SLICE_AREA = PIZZA_AREA / 8;

int main(int argc, char** argv) {
    std::cout << "Problem 1" << std::endl;
    std::cout << "Area = " << RECT_AREA << "m^2" << std::endl;
    std::cout << "Problem 2" << std::endl;
    std::cout << "Product Price: $" << std::setprecision(2) << std::fixed << PRODUCT_PRICE << "." << std::endl;
    std::cout << "Sales Tax: $" << std::setprecision(2) << std::fixed << SALES_TAX << "." << std::endl;
    std::cout << "Final Cost: $" << std::setprecision(2) << std::fixed << FINAL_COST << "." << std::endl;
    std::cout << "Problem 3" << std::endl;
    std::cout << "Pizza Area: " << PIZZA_AREA << "ft^2." << std::endl;
    std::cout << "Problem 4" << std::endl;
    std::cout << "Pizza Slice Area: " << PIZZA_SLICE_AREA << "ft^2." << std::endl;
    return 0;
}