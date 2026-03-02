#define __STDC_WANT_LIB_EXT1__ 1
#include <cstring>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

struct FatPtr {
public:
    void* data;
    size_t size;

    FatPtr(const void* a_data, size_t a_size) {
        this->data = malloc(a_size);
        this->size = a_size;
        
        #ifdef __STDC_LIB_EXT1__
        memcpy_s(this->data, a_size, a_data, a_size);
        #else
        memcpy(this->data, a_data, a_size);
        #endif
    }

    void SetData(const void* a_data, size_t a_size) {
        this->data = realloc(this->data, a_size);
        this->size = a_size;

        #ifdef __STDC_LIB_EXT1__
        memcpy_s(this->data, a_size, a_data, a_size);
        #else
        memcpy(this->data, a_data, a_size);
        #endif
    }

    ~FatPtr() {
        free(this->data);
    }
};

struct Interpreter {
public:
    std::unordered_map<std::string, FatPtr> variables = std::unordered_map<std::string, FatPtr>();

    Interpreter(void) {

    }

    void Parse(std::string a_statement) {
        std::string register0 = "";
        std::string register1 = "";
        std::string register2 = "";
        std::string register3 = "";
        std::string register4 = "";
        std::string register5 = "";
        int32_t mode = 0;
        bool inString = false;
        bool escaped = false;
        for (char character : a_statement) {
            switch (mode) {

            }
        }
    }
};

int main(int argc, char** argv) {


    return 0;
}