// CSC 134
// M2T2
// Daley Ottersbach
// 1/14/2026

#include <iostream>
#include <iomanip>

const float PRODUCT_PRICE = 5.99f;
const float SALES_TAX_PERCENT = 0.08f;
const float SALES_TAX = PRODUCT_PRICE * SALES_TAX_PERCENT;
const float FINAL_COST = PRODUCT_PRICE + SALES_TAX;

int main(int argc, char** argv) {
    std::cout << std::setprecision(2) << std::fixed;
    std::cout << "RECEIPT" << std::endl;
    std::cout << "Product Price: $" << PRODUCT_PRICE << "." << std::endl;
    std::cout << (SALES_TAX_PERCENT * 100) << "\% Sales Tax: $" << SALES_TAX << "." << std::endl;
    std::cout << "Final Cost: $" << FINAL_COST << "." << std::endl;
    return 0;
}