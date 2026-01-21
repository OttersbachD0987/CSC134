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
    std::cout << "RECEIPT" << std::endl;
    std::cout << "Product Price: $" << std::setprecision(2) << std::fixed << PRODUCT_PRICE << "." << std::endl;
    std::cout << (SALES_TAX_PERCENT * 100) << "\% Sales Tax: $" << std::setprecision(2) << std::fixed << SALES_TAX << "." << std::endl;
    std::cout << "Final Cost: $" << std::setprecision(2) << std::fixed << FINAL_COST << "." << std::endl;
    return 0;
}