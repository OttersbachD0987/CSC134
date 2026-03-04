#define __STDC_WANT_LIB_EXT1__ 1
#include <cstring>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

size_t SplitString(const std::string& a_string, std::string_view a_seperator, std::vector<std::string>& a_vector) {
    int32_t previous = 0;
    int32_t cur = 0;
    while ((cur = a_string.find(a_seperator, cur)) < a_string.size()) {
        a_vector.push_back(a_string.substr(previous, cur - previous));
        //std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
        previous = ++cur;
    }
    a_vector.push_back(a_string.substr(previous, cur - previous));
    //std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
    return a_vector.size();
}

void SanitizeString(std::string& a_string) {
    size_t startOff = 0;
    size_t endOff = 0;
    bool startDone = false;
    for (size_t i = 0; i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case ' ':
            case '\n':
            case '\t':
            case '\v':
                break;
            default:
                if (startDone) {
                    endOff = i;
                } else {
                    startOff = i;
                    startDone = true;
                }
                break;
        }
    }
    a_string = a_string.substr(startOff, endOff - startOff);
}

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

enum struct ReaderMode {
    READ,
    VAR_NAME,
    OBJECT,
    EXPRESSION,
    ARGS,
    FALLTHROUGH,
};

enum struct OperatorEvaluation {
    NONE,
    ADDI,
    SUBT,
    MULT,
    DIVI,
    MODU,
    NOT,
    AND,
    OR,
    GT,
    GTE,
    LT,
    LTE,
    EQ,
    NEQ,
};


enum struct RegisterType {
    ERROR,
    STRING,
    INT,
    FLOAT,
    BOOL,
};

struct InterpreterContext {
public:
    std::vector<std::string> stringRegisters = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    std::vector<int32_t> intRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<float> floatRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<bool> boolRegisters = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
    std::vector<std::string> context = std::vector<std::string>();
    size_t stringRegister = 0;
    size_t intRegister = -1;
    size_t floatRegister = -1;
    size_t boolRegister = -1;
    size_t targetLevel = 0;
    size_t currentLevel = 0;

    RegisterType SetIntRegister(OperatorEvaluation operatorEvaluation, int32_t a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                intRegisters[++intRegister] = a_value;
                return RegisterType::INT;
            case OperatorEvaluation::ADDI:
                intRegisters[intRegister] = intRegisters[intRegister] + a_value;
                return RegisterType::INT;
            case OperatorEvaluation::SUBT:
                intRegisters[intRegister] = intRegisters[intRegister] - a_value;
                return RegisterType::INT;
            case OperatorEvaluation::MULT:
                intRegisters[intRegister] = intRegisters[intRegister] * a_value;
                return RegisterType::INT;
            case OperatorEvaluation::DIVI:
                intRegisters[intRegister] = intRegisters[intRegister] / a_value;
                return RegisterType::INT;
            case OperatorEvaluation::MODU:
                intRegisters[intRegister] = intRegisters[intRegister] % a_value;
                return RegisterType::INT;
            case OperatorEvaluation::GT:
                return SetBoolRegister(OperatorEvaluation::NONE, intRegisters[intRegister] > a_value);
            case OperatorEvaluation::GTE:
                return SetBoolRegister(OperatorEvaluation::NONE, intRegisters[intRegister] >= a_value);
            case OperatorEvaluation::LT:
                return SetBoolRegister(OperatorEvaluation::NONE, intRegisters[intRegister] < a_value);
            case OperatorEvaluation::LTE:
                return SetBoolRegister(OperatorEvaluation::NONE, intRegisters[intRegister] <= a_value);
            case OperatorEvaluation::EQ:
                return SetBoolRegister(OperatorEvaluation::NONE, intRegisters[intRegister] == a_value);
            case OperatorEvaluation::NEQ:
                return SetBoolRegister(OperatorEvaluation::NONE, intRegisters[intRegister] != a_value);
        }
        return RegisterType::ERROR;
    }

    RegisterType SetFloatRegister(OperatorEvaluation operatorEvaluation, float a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                floatRegisters[++floatRegister] = a_value;
                return RegisterType::FLOAT;
            case OperatorEvaluation::ADDI:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] + a_value;
                return RegisterType::FLOAT;
            case OperatorEvaluation::SUBT:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] - a_value;
                return RegisterType::FLOAT;
            case OperatorEvaluation::MULT:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] * a_value;
                return RegisterType::FLOAT;
            case OperatorEvaluation::DIVI:
                floatRegisters[floatRegister] = floatRegisters[floatRegister] / a_value;
                return RegisterType::FLOAT;
            case OperatorEvaluation::GT:
                return SetBoolRegister(OperatorEvaluation::NONE, floatRegisters[floatRegister] > a_value);
            case OperatorEvaluation::GTE:
                return SetBoolRegister(OperatorEvaluation::NONE, floatRegisters[floatRegister] >= a_value);
            case OperatorEvaluation::LT:
                return SetBoolRegister(OperatorEvaluation::NONE, floatRegisters[floatRegister] < a_value);
            case OperatorEvaluation::LTE:
                return SetBoolRegister(OperatorEvaluation::NONE, floatRegisters[floatRegister] <= a_value);
            case OperatorEvaluation::EQ:
                return SetBoolRegister(OperatorEvaluation::NONE, floatRegisters[floatRegister] == a_value);
            case OperatorEvaluation::NEQ:
                return SetBoolRegister(OperatorEvaluation::NONE, floatRegisters[floatRegister] != a_value);
        }
        return RegisterType::ERROR;
    }

    RegisterType SetBoolRegister(OperatorEvaluation operatorEvaluation, bool a_value) {
        switch (operatorEvaluation) {
            case OperatorEvaluation::NONE:
                boolRegisters[++boolRegister] = a_value;
                return RegisterType::BOOL;
            case OperatorEvaluation::NOT:
                boolRegisters[++boolRegister] = !a_value;
                return RegisterType::BOOL;
            case OperatorEvaluation::AND:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] && a_value;
                return RegisterType::BOOL;
            case OperatorEvaluation::OR:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] || a_value;
                return RegisterType::BOOL;
            case OperatorEvaluation::EQ:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] == a_value;
                return RegisterType::BOOL;
            case OperatorEvaluation::NEQ:
                boolRegisters[boolRegister] = boolRegisters[boolRegister] != a_value;
                return RegisterType::BOOL;
        }
        return RegisterType::ERROR;
    }
};

struct Interpreter {
public:
    std::unordered_map<std::string, FatPtr> variables = std::unordered_map<std::string, FatPtr>();

    Interpreter(void) {

    }

    void ParseStatement(std::string a_statement) {
        InterpreterContext context = InterpreterContext();
        OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
        std::vector<ReaderMode> mode = {ReaderMode::READ};
        bool ifTrue = false;
        bool inString = false;
        bool escaped = false;
        for (char character : a_statement) {
            switch (mode.back()) {
                case ReaderMode::READ:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '{':
                            ++context.currentLevel;
                            break;
                        case '}':
                            --context.currentLevel;
                            break;
                        case '.':
                            if (context.stringRegisters[context.stringRegister] == "player") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "math") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "console") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            }
                            break;
                        case '(':
                            if (context.stringRegisters[context.stringRegister] == "if") {
                                context.stringRegisters[context.stringRegister] = "";
                                context.targetLevel = context.currentLevel;
                                mode.push_back(ReaderMode::EXPRESSION);
                            }
                            break;
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::OBJECT:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "maxMana") {
                                    context.SetIntRegister(operatorEvaluation, 5);
                                } else if (context.stringRegisters[context.stringRegister] == "curMana") {
                                    context.SetIntRegister(operatorEvaluation, 2);
                                }
                            }
                            mode.pop_back();
                            break;
                        }
                        case '(': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "HasPerk") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Hurt") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "RegainMana") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            } else if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            } else if (object == "console") {
                                if (context.stringRegisters[context.stringRegister] == "Print") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::ARGS:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '.':
                            if (context.stringRegisters[context.stringRegister] == "player") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "math") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "console") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            }
                            break;
                        case ')': {
                            std::vector<std::string> args = std::vector<std::string>();
                            size_t argc = SplitString(context.stringRegisters[context.stringRegister], ",", args);
                            for (size_t i = 0; i < argc; ++i) {
                                SanitizeString(args[i]);
                            }
                            std::string object = context.stringRegisters[context.stringRegister - 2];
                            std::string method = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (method == "HasPerk") {
                                    if (argc == 1) {
                                        if (args[0] == "ARCANE_EYES") {
                                            context.SetBoolRegister(operatorEvaluation, true);
                                        } else {
                                            std::cout << "[ERROR]: " << args[0] << " is not a valid perk id." << std::endl;
                                        }
                                    } else {
                                        std::cout << "[ERROR]: player.HasPerk must have 1 argument, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Hurt") {
                                    if (argc == 1) {
                                        int32_t damage = ParseIntExpression(context, args[0]);
                                        std::cout << "Dealt " << damage << " to the player." << std::endl;
                                    } else {
                                        std::cout << "[ERROR]: player.Hurt must have 1 argument, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "RegainMana") {
                                    if (argc == 1) {
                                        int32_t mana = ParseIntExpression(context, args[0]);
                                        std::cout << "Gave " << mana << " to the player." << std::endl;
                                    } else {
                                        std::cout << "[ERROR]: player.RegainMana must have 1 argument, this call has " << argc << "." << std::endl;
                                    }
                                }
                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                            } else if (object == "math") {
                                if (method == "Min") {
                                    if (argc == 2) {
                                        context.SetIntRegister(operatorEvaluation, std::min<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                    } else {
                                        std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Max") {
                                    if (argc == 2) {
                                        context.SetIntRegister(operatorEvaluation, std::max<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                    } else {
                                        std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Clamp") {
                                    if (argc == 3) {
                                        context.SetIntRegister(operatorEvaluation, std::clamp<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1]), ParseIntExpression(context, args[2])));
                                    } else {
                                        std::cout << "[ERROR]: math.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
                                    }
                                }
                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                            } else if (object == "console") {
                                if (argc == 1) {
                                        std::cout << FormatString(context, args[0]);
                                    } else {
                                        std::cout << "[ERROR]: console.Print must have 1 argument, this call has " << argc << "." << std::endl;
                                    }
                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                            }
                            mode.pop_back();
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
            }
        }
    }

    int32_t ParseIntExpression(InterpreterContext& a_context, std::string a_expression) {
        InterpreterContext context = InterpreterContext();
        std::vector<ReaderMode> mode = {ReaderMode::READ};
        OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
        bool inString = false;
        bool escaped = false;
        for (char character : a_expression) {
            switch (mode.back()) {
                case ReaderMode::READ:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '{':
                        case '}':
                            std::cout << "[ERROR] Invalid characters." << std::endl;
                            break;
                        case '.':
                            if (context.stringRegisters[a_context.stringRegister] == "player") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (a_context.stringRegisters[a_context.stringRegister] == "math") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            }
                            break;
                        case '+':
                            operatorEvaluation = OperatorEvaluation::ADDI;
                            break;
                        case '-':
                            operatorEvaluation = OperatorEvaluation::SUBT;
                            break;
                        case '*':
                            operatorEvaluation = OperatorEvaluation::MULT;
                            break;
                        case '/':
                            operatorEvaluation = OperatorEvaluation::DIVI;
                            break;
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::OBJECT:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "maxMana") {
                                    context.SetIntRegister(operatorEvaluation, 5);
                                } else if (context.stringRegisters[context.stringRegister] == "curMana") {
                                    context.SetIntRegister(operatorEvaluation, 2);
                                }
                            }
                            break;
                        }
                        case '(': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::ARGS:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case ')': {
                            std::vector<std::string> args = std::vector<std::string>();
                            size_t argc = SplitString(context.stringRegisters[context.stringRegister], ",", args);
                            for (size_t i = 0; i < argc; ++i) {
                                SanitizeString(args[i]);
                            }
                            std::string object = context.stringRegisters[context.stringRegister - 2];
                            std::string method = context.stringRegisters[context.stringRegister - 1];
                            if (object == "math") {
                                if (method == "Min") {
                                    if (argc == 2) {
                                        context.SetIntRegister(operatorEvaluation, std::min<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                    } else {
                                        std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Max") {
                                    if (argc == 2) {
                                        context.SetIntRegister(operatorEvaluation, std::max<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                    } else {
                                        std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Clamp") {
                                    if (argc == 3) {
                                        context.SetIntRegister(operatorEvaluation, std::clamp<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1]), ParseIntExpression(context, args[2])));
                                    } else {
                                        std::cout << "[ERROR]: math.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
                                    }
                                }
                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                            }


                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
            }
        }

        return context.intRegisters[context.intRegister];
    }

    std::string FormatString(InterpreterContext& a_context, std::string a_string) {
        InterpreterContext context = InterpreterContext();
        std::string result = "";
        std::vector<ReaderMode> mode = {ReaderMode::READ};
        OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
        RegisterType registerType = RegisterType::INT;
        bool inString = true;
        bool escaped = false;
        for (char character : a_string) {
            switch (mode.back()) {
                case ReaderMode::READ:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            if (inString) {
                                result += character;
                            }
                            break;
                        case '{':
                            if (inString) {
                                if (escaped) {
                                    result += character;
                                    escaped = false;
                                } else {
                                    inString = false;
                                }
                            } else {
                                std::cout << "[ERROR] Invalid characters." << std::endl;
                            }
                            break;
                        case '}':
                            if (inString) {
                                if (escaped) {
                                    result += character;
                                    escaped = false;
                                } else {
                                    std::cout << "[ERROR] Invalid characters." << std::endl;
                                }
                            } else {
                                inString = false;
                                // Match Register Type?
                                switch (registerType) {
                                    case RegisterType::STRING:
                                        result += context.stringRegisters[context.stringRegister];
                                        break;
                                    case RegisterType::INT:
                                        result += std::to_string(context.intRegisters[context.intRegister]);
                                        break;
                                    case RegisterType::FLOAT:
                                        result += std::to_string(context.floatRegisters[context.floatRegister]);
                                        break;
                                    case RegisterType::BOOL:
                                        result += context.boolRegisters[context.boolRegister] ? "true" : "false";
                                        break;
                                }
                            }
                            break;
                        case '.':
                            if (context.stringRegisters[a_context.stringRegister] == "player") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (a_context.stringRegisters[a_context.stringRegister] == "math") {
                                ++context.stringRegister;
                                mode.push_back(ReaderMode::OBJECT);
                            }
                            break;
                        case '+':
                            operatorEvaluation = OperatorEvaluation::ADDI;
                            break;
                        case '-':
                            operatorEvaluation = OperatorEvaluation::SUBT;
                            break;
                        case '*':
                            operatorEvaluation = OperatorEvaluation::MULT;
                            break;
                        case '/':
                            operatorEvaluation = OperatorEvaluation::DIVI;
                            break;
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::OBJECT:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                        case ')': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "maxMana") {
                                    registerType = context.SetIntRegister(operatorEvaluation, 5);
                                } else if (context.stringRegisters[context.stringRegister] == "curMana") {
                                    registerType = context.SetIntRegister(operatorEvaluation, 2);
                                }
                            }
                            break;
                        }
                        case '(': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    ++context.stringRegister;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::ARGS:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case ')': {
                            std::vector<std::string> args = std::vector<std::string>();
                            size_t argc = SplitString(context.stringRegisters[context.stringRegister], ",", args);
                            for (size_t i = 0; i < argc; ++i) {
                                SanitizeString(args[i]);
                            }
                            std::string object = context.stringRegisters[context.stringRegister - 2];
                            std::string method = context.stringRegisters[context.stringRegister - 1];
                            if (object == "math") {
                                if (method == "Min") {
                                    if (argc == 2) {
                                        registerType = context.SetIntRegister(operatorEvaluation, std::min<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                    } else {
                                        std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Max") {
                                    if (argc == 2) {
                                        registerType = context.SetIntRegister(operatorEvaluation, std::max<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                    } else {
                                        std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Clamp") {
                                    if (argc == 3) {
                                        registerType = context.SetIntRegister(operatorEvaluation, std::clamp<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1]), ParseIntExpression(context, args[2])));
                                    } else {
                                        std::cout << "[ERROR]: math.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
                                    }
                                }
                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                            }


                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
            }
        }
    }
};

int main(int argc, char** argv) {
    //std::string testString = "A, bcde, efef, hpphuweaf, aew";
    //std::vector<std::string> split = std::vector<std::string>();
    //size_t size = SplitString(testString, ",", split);
    //std::cout << "Size: " << size << std::endl;

    return 0;
}