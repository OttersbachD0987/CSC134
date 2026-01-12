// CSC 134
// M2LAB1
// Daley Ottersbach
// 1/12/2026

#include <iostream>

const float PRODUCT_PRICE = 5.99f;
const float SALES_TAX_PERCENT = 0.08f;
const float SALES_TAX = PRODUCT_PRICE * SALES_TAX_PERCENT;
const float FINAL_COST = PRODUCT_PRICE + SALES_TAX;

int main() {
    std::cout << "RECEIPT" << std::endl;
    std::cout << "Product Price: $" << PRODUCT_PRICE << "." << std::endl;
    std::cout << "Sales Tax: $" << SALES_TAX << "." << std::endl;
    std::cout << "Final Cost: $" << FINAL_COST << "." << std::endl;
    return 0;
}

/*
Ask the user to input the crate’s length.
Ask the user to input the crate’s width.
Ask the user to input the crate’s height.
Calculate the crate’s volume.
Calculate the cost of building the crate.
Calculate the customer’s charge for the crate.
Calculate the profit made from the crate.
Display the crate’s volume.
Display the cost of building the crate.
Display the customer’s charge for the crate.
Display the profit made from the crate.
*/