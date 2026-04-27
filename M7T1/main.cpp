// CSC 134
// M7T1
// Daley Ottersbach
// 4/27/2026

#include <iostream>
#include <string>

class Restaurant {
public:
    std::string name;
    float rating;

    Restaurant(std::string_view a_name, float a_rating) {
        this->name = a_name;
        this->rating = a_rating;
    }
};

int main(int argc, char** argv) {
    std::cout << "M7T1" << std::endl;

    Restaurant restaurant1 = Restaurant("Abra", 2.0);
    Restaurant restaurant2 = Restaurant("Abra", 4.0);
    Restaurant restaurant3 = Restaurant("Abra", 3.0);
}