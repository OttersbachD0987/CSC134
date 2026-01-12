#include <iostream>

template <typename T>
void Input(const char* a_prompt, T& a_value) {
    std::cout << a_prompt;
    std::cin >> a_value;
}