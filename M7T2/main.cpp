// CSC 134
// M7T1
// Daley Ottersbach
// 4/27/2026

#include <iostream>
#include <string>
#include "Rectangle.h"


void Input(std::string& a_val) {
    std::getline(std::cin, a_val);
}

int main(int argc, char** argv) {
    std::string name = "Bob";
    std::string* p_name = &name;
    
    std::cout << "name    = " << name << std::endl;
    std::cout << "p_name  = " << p_name << std::endl;
    std::cout << "*p_name = " << *p_name << std::endl;

    std::cout << "New Name: ";
    Input(name);

    std::cout << "name    = " << name << std::endl;
    std::cout << "p_name  = " << p_name << std::endl;
    std::cout << "*p_name = " << *p_name << std::endl;

    *p_name = "Ana";

    std::cout << "name    = " << name << std::endl;
    std::cout << "p_name  = " << p_name << std::endl;
    std::cout << "*p_name = " << *p_name << std::endl;

    Rectangle ra = Rectangle{2, 4};
}