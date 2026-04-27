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
    std::cout << "Restaurant Reviews" << std::endl;

    Restaurant restaurant1 = Restaurant("Cowfish", 4.7);
    std::cout << restaurant1.name << "\n" << restaurant1.rating << std::endl;

    Restaurant restaurant2 = Restaurant("Five Guys", 4.3);
    std::cout << "Restaurant Name: ";
    std::cin >> restaurant2.name;
    std::cout << "Restaurant Rating: ";
    std::cin >> restaurant2.rating;
    std::cout << restaurant2.name << "\n" << restaurant2.rating << std::endl

    
}