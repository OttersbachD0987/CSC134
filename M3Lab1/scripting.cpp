#define __STDC_WANT_LIB_EXT1__ 1
#include <cstring>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <cmath>

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
    VAR_TYPE,
    VAR_VALUE,
    VAR_EXPR,
    OBJECT,
    EXPRESSION,
    ARGS,
    FALLTHROUGH,
    FINISHED,
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
        std::vector<std::pair<int32_t, bool>> ifTrue = {};
        int32_t ifLevel = 0;
        bool inString = false;
        bool escaped = false;
        int32_t embedLevel = 0;
        for (char character : a_statement) {
            //std::cout << character << " : " << std::to_string((int32_t)mode.back()) << std::endl;
            switch (mode.back()) {
                case ReaderMode::READ:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '<':
                            context.stringRegister += 1;
                            mode.push_back(ReaderMode::VAR_TYPE);
                            break;
                        case ';':
                            if (context.stringRegisters[context.stringRegister] == "end") {
                                if (ifTrue.size() > 0 && ifTrue.back().first == context.currentLevel) {
                                    ifTrue.pop_back();
                                } else {
                                    mode.push_back(ReaderMode::FINISHED);
                                }
                            }
                            break;
                        case '{':
                            if (context.stringRegisters[context.stringRegister] == "else") {
                                if (ifTrue.back().second) {
                                    ifTrue.pop_back();
                                    mode.push_back(ReaderMode::FALLTHROUGH);
                                } else {
                                    ++context.currentLevel;
                                }
                            } else {
                                ++context.currentLevel;
                            }
                            break;
                        case '}':
                            --context.currentLevel;
                            while (ifTrue.back().first > context.currentLevel) {
                                ifTrue.pop_back();
                            }
                            break;
                        case '.':
                            if (context.stringRegisters[context.stringRegister] == "player") {
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "math") {
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "console") {
                                context.stringRegister += 1;
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
                case ReaderMode::VAR_TYPE:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '>': {
                            mode.pop_back();
                            mode.push_back(ReaderMode::VAR_VALUE);
                            context.stringRegister += 1;
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::VAR_VALUE:
                    switch (character) {
                        case ';':
                            std::cout << "[ERROR] Semicolon before variable expression provided." << std::endl;
                            break;
                        case '=': {
                            mode.pop_back();
                            mode.push_back(ReaderMode::VAR_EXPR);
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                case ReaderMode::VAR_EXPR:
                    switch (character) {
                        case ' ':
                            if (inString) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                } else {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case ';': {
                            if (inString) { 
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                mode.pop_back();
                                std::string varName = context.stringRegisters[context.stringRegister - 2];
                                std::string varType = context.stringRegisters[context.stringRegister - 1];
                                if (varType == "uint32") {
                                    int32_t value = ParseIntExpression(context, context.stringRegisters[context.stringRegister]);
                                    variables.insert_or_assign(varName, FatPtr((void*)&value, sizeof(int32_t)));
                                } else if (varType == "bool") {
                                    bool value = ParseBoolExpression(context, context.stringRegisters[context.stringRegister]);
                                    variables.insert_or_assign(varName, FatPtr((void*)&value, sizeof(bool)));
                                } else if (varType == "string") {
                                    std::cout << "String is not a supported data type." << std::endl;
                                }

                                for (size_t i = 0; i < 3; ++i) {
                                    context.stringRegisters[context.stringRegister--] = "";
                                }
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
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
                            std::cout << "Operating on member " << context.stringRegisters[context.stringRegister] << std::endl;
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "maxMana") {
                                    context.SetIntRegister(operatorEvaluation, 5);
                                    operatorEvaluation = OperatorEvaluation::NONE;
                                } else if (context.stringRegisters[context.stringRegister] == "curMana") {
                                    context.SetIntRegister(operatorEvaluation, 2);
                                    operatorEvaluation = OperatorEvaluation::NONE;
                                }
                            }
                            mode.pop_back();
                            break;
                        }
                        case '(': {
                            std::cout << "Operating on function " << context.stringRegisters[context.stringRegister] << std::endl;
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "HasPerk") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Hurt") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "RegainMana") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            } else if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            } else if (object == "console") {
                                if (context.stringRegisters[context.stringRegister] == "Print") {
                                    context.stringRegister += 1;
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
                            if (inString) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '"':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                } else {
                                    inString = false;
                                }
                                escaped = false;
                            } else {
                                inString = true;
                            }
                            break;
                        case '\\':
                            if (inString) {
                                if (escaped) {
                                    context.stringRegisters[context.stringRegister] += character;
                                    escaped = false;
                                } else {
                                    escaped = true;
                                }
                            }
                            break;
                        case '.':
                            if (!inString) {
                                if (context.stringRegisters[context.stringRegister] == "player") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::OBJECT);
                                } else if (context.stringRegisters[context.stringRegister] == "math") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::OBJECT);
                                } else if (context.stringRegisters[context.stringRegister] == "console") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::OBJECT);
                                }
                            }
                        case ')': {
                            if (!inString) {
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
                                                operatorEvaluation = OperatorEvaluation::NONE;
                                            } else {
                                                std::cout << "[ERROR]: " << args[0] << " is not a valid perk id." << std::endl;
                                            }
                                        } else {
                                            std::cout << "[ERROR]: player.HasPerk must have 1 argument, this call has " << argc << "." << std::endl;
                                        }
                                    } else if (method == "Hurt") {
                                        if (argc == 1) {
                                            int32_t damage = ParseIntExpression(context, args[0]);
                                            operatorEvaluation = OperatorEvaluation::NONE;
                                            std::cout << "Dealt " << damage << " to the player." << std::endl;
                                        } else {
                                            std::cout << "[ERROR]: player.Hurt must have 1 argument, this call has " << argc << "." << std::endl;
                                        }
                                    } else if (method == "RegainMana") {
                                        if (argc == 1) {
                                            int32_t mana = ParseIntExpression(context, args[0]);
                                            operatorEvaluation = OperatorEvaluation::NONE;
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
                                            operatorEvaluation = OperatorEvaluation::NONE;
                                        } else {
                                            std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                                        }
                                    } else if (method == "Max") {
                                        if (argc == 2) {
                                            context.SetIntRegister(operatorEvaluation, std::max<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                            operatorEvaluation = OperatorEvaluation::NONE;
                                        } else {
                                            std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                                        }
                                    } else if (method == "Clamp") {
                                        if (argc == 3) {
                                            context.SetIntRegister(operatorEvaluation, std::clamp<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1]), ParseIntExpression(context, args[2])));
                                            operatorEvaluation = OperatorEvaluation::NONE;
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
                            }
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                            break;
                    }
                    break;
                case ReaderMode::EXPRESSION:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            break;
                        case '(':
                            embedLevel += 1;
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                        case ')': {
                            if (embedLevel-- == 0) {
                                ifTrue.push_back(std::pair<int32_t, bool>(context.currentLevel, ParseBoolExpression(context, context.stringRegisters[context.stringRegister])));
                                mode.pop_back();
                                if (!ifTrue.back().second) {
                                    mode.push_back(ReaderMode::FALLTHROUGH);
                                }
                            } else {
                                context.stringRegisters[context.stringRegister] += character;
                            }
                            break;
                        }
                        default:
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::FALLTHROUGH:
                    switch (character) {
                        case '\\':
                            escaped = !escaped;
                            break;
                        case '}':
                            if (!escaped) {
                                --context.currentLevel;
                                if (context.currentLevel == context.targetLevel) {
                                    mode.pop_back();
                                }
                            }
                            escaped = false;
                            break;
                        default:
                            escaped = false;
                            break;
                    }
                    break;
            }
        }
    }

    int32_t ParseIntExpression(InterpreterContext& a_context, std::string a_expression) {
        std::cout << "Parsing Int Expression: \n" << a_expression << std::endl;
        InterpreterContext context = InterpreterContext();
        std::vector<ReaderMode> mode = {ReaderMode::READ};
        OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
        bool inString = false;
        bool escaped = false;
        bool justOperated = false;
        for (char character : a_expression) {
            switch (mode.back()) {
                case ReaderMode::READ:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            if (context.stringRegisters[context.stringRegister] == "")
                            justOperated = false;
                            break;
                        case '@':
                            context.stringRegister += 1;
                            mode.push_back(ReaderMode::VAR_NAME);
                            break;
                        case '{':
                        case '}':
                            justOperated = false;
                            std::cout << "[ERROR] Invalid characters." << std::endl;
                            break;
                        case '.':
                            justOperated = false;
                            if (context.stringRegisters[context.stringRegister] == "player") {
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "math") {
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            }
                            break;
                        case '+':
                            operatorEvaluation = OperatorEvaluation::ADDI;
                            justOperated = true;
                            break;
                        case '-':
                            operatorEvaluation = OperatorEvaluation::SUBT;
                            justOperated = true;
                            break;
                        case '*':
                            operatorEvaluation = OperatorEvaluation::MULT;
                            justOperated = true;
                            break;
                        case '/':
                            operatorEvaluation = OperatorEvaluation::DIVI;
                            justOperated = true;
                            break;
                        case '!':
                            operatorEvaluation = OperatorEvaluation::NOT;
                            justOperated = true;
                            break;
                        case '<':
                            operatorEvaluation = OperatorEvaluation::LT;
                            justOperated = true;
                            break;
                        case '>':
                            operatorEvaluation = OperatorEvaluation::GT;
                            justOperated = true;
                            break;
                        case '=':
                            switch (operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    operatorEvaluation = OperatorEvaluation::EQ;
                                    break;
                                case OperatorEvaluation::NOT:
                                    operatorEvaluation = OperatorEvaluation::NEQ;
                                    break;
                                case OperatorEvaluation::LT:
                                    operatorEvaluation = OperatorEvaluation::LTE;
                                    break;
                                case OperatorEvaluation::GT:
                                    operatorEvaluation = OperatorEvaluation::GTE;
                                    break;
                            }
                            justOperated = true;
                            break;
                        default:
                            justOperated = false;
                            context.stringRegisters[context.stringRegister] += character;
                            break;
                    }
                    break;
                case ReaderMode::VAR_NAME:
                    switch (character) {
                        case '@': {
                            std::string varType = context.stringRegisters[context.stringRegister - 1];
                            if (varType == "uint32") {
                                context.SetIntRegister(operatorEvaluation, *((int32_t*)variables.at(context.stringRegisters[context.stringRegister]).data));
                                operatorEvaluation = OperatorEvaluation::NONE;
                            } else if (varType == "bool") {
                                context.SetBoolRegister(operatorEvaluation, *((bool*)variables.at(context.stringRegisters[context.stringRegister]).data));
                                operatorEvaluation = OperatorEvaluation::NONE;
                            }

                            for (size_t i = 0; i < 2; ++i) {
                                context.stringRegisters[context.stringRegister--] = "";
                            }
                            mode.pop_back();
                            break;
                        }
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
                                    operatorEvaluation = OperatorEvaluation::NONE;
                                } else if (context.stringRegisters[context.stringRegister] == "curMana") {
                                    context.SetIntRegister(operatorEvaluation, 2);
                                    operatorEvaluation = OperatorEvaluation::NONE;
                                }
                            }
                            mode.pop_back();
                            break;
                        }
                        case '(': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    context.stringRegister += 1;
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
                                        operatorEvaluation = OperatorEvaluation::NONE;
                                    } else {
                                        std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Max") {
                                    if (argc == 2) {
                                        context.SetIntRegister(operatorEvaluation, std::max<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                        operatorEvaluation = OperatorEvaluation::NONE;
                                    } else {
                                        std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Clamp") {
                                    if (argc == 3) {
                                        context.SetIntRegister(operatorEvaluation, std::clamp<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1]), ParseIntExpression(context, args[2])));
                                        operatorEvaluation = OperatorEvaluation::NONE;
                                    } else {
                                        std::cout << "[ERROR]: math.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
                                    }
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

        return context.intRegisters[context.intRegister];
    }

    int32_t ParseBoolExpression(InterpreterContext& a_context, std::string a_expression) {
        std::cout << "Parsing Bool Expression: \n" << a_expression << std::endl;
        InterpreterContext context = InterpreterContext();
        std::vector<ReaderMode> mode = {ReaderMode::READ};
        OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
        bool inString = false;
        bool escaped = false;
        bool justOperated = false;
        for (char character : a_expression) {
            switch (mode.back()) {
                case ReaderMode::READ:
                    switch (character) {
                        case ' ':
                        case '\n':
                        case '\t':
                        case '\v':
                            if (context.stringRegisters[context.stringRegister] == "")
                            justOperated = false;
                            break;
                        case '{':
                        case '}':
                            justOperated = false;
                            std::cout << "[ERROR] Invalid characters." << std::endl;
                            break;
                        case '.':
                            justOperated = false;
                            std::cout << "Operating on object " << context.stringRegisters[context.stringRegister] << std::endl;
                            if (context.stringRegisters[context.stringRegister] == "player") {
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (context.stringRegisters[context.stringRegister] == "math") {
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            }
                            break;
                        case '+':
                            operatorEvaluation = OperatorEvaluation::ADDI;
                            justOperated = true;
                            break;
                        case '-':
                            operatorEvaluation = OperatorEvaluation::SUBT;
                            justOperated = true;
                            break;
                        case '*':
                            operatorEvaluation = OperatorEvaluation::MULT;
                            justOperated = true;
                            break;
                        case '/':
                            operatorEvaluation = OperatorEvaluation::DIVI;
                            justOperated = true;
                            break;
                        case '!':
                            operatorEvaluation = OperatorEvaluation::NOT;
                            justOperated = true;
                            break;
                        case '<':
                            operatorEvaluation = OperatorEvaluation::LT;
                            justOperated = true;
                            break;
                        case '>':
                            operatorEvaluation = OperatorEvaluation::GT;
                            justOperated = true;
                            break;
                        case '=':
                            switch (operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    operatorEvaluation = OperatorEvaluation::EQ;
                                    break;
                                case OperatorEvaluation::NOT:
                                    operatorEvaluation = OperatorEvaluation::NEQ;
                                    break;
                                case OperatorEvaluation::LT:
                                    operatorEvaluation = OperatorEvaluation::LTE;
                                    break;
                                case OperatorEvaluation::GT:
                                    operatorEvaluation = OperatorEvaluation::GTE;
                                    break;
                            }
                            justOperated = true;
                            break;
                        default:
                            justOperated = false;
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
                            std::cout << "Operating on member " << context.stringRegisters[context.stringRegister] << std::endl;
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
                            std::cout << "Operating on function " << context.stringRegisters[context.stringRegister] << std::endl;
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "player") {
                                if (context.stringRegisters[context.stringRegister] == "HasPerk") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                }
                            } else if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    context.stringRegister += 1;
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
                            std::cout << "We have finished with " << object << "." << method << ", there are " << argc << " arguments." << std::endl;
                            if (object == "player") {
                                    if (method == "HasPerk") {
                                        if (argc == 1) {
                                            if (args[0] == "ARCANE_EYES") {
                                                context.SetBoolRegister(operatorEvaluation, true);
                                                operatorEvaluation = OperatorEvaluation::NONE;
                                            } else {
                                                std::cout << "[ERROR]: " << args[0] << " is not a valid perk id." << std::endl;
                                            }
                                        } else {
                                            std::cout << "[ERROR]: player.HasPerk must have 1 argument, this call has " << argc << "." << std::endl;
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

        std::cout << "Returning: " << (context.boolRegisters[context.boolRegister] ? "true" : "false") << std::endl;

        return context.boolRegisters[context.boolRegister];
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
                                context.stringRegister += 1;
                                mode.push_back(ReaderMode::OBJECT);
                            } else if (a_context.stringRegisters[a_context.stringRegister] == "math") {
                                context.stringRegister += 1;
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
                                    operatorEvaluation = OperatorEvaluation::NONE;
                                } else if (context.stringRegisters[context.stringRegister] == "curMana") {
                                    registerType = context.SetIntRegister(operatorEvaluation, 2);
                                    operatorEvaluation = OperatorEvaluation::NONE;
                                }
                            }
                            break;
                        }
                        case '(': {
                            std::string object = context.stringRegisters[context.stringRegister - 1];
                            if (object == "math") {
                                if (context.stringRegisters[context.stringRegister] == "Min") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Max") {
                                    context.stringRegister += 1;
                                    mode.push_back(ReaderMode::ARGS);
                                } else if (context.stringRegisters[context.stringRegister] == "Clamp") {
                                    context.stringRegister += 1;
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
                                        operatorEvaluation = OperatorEvaluation::NONE;
                                    } else {
                                        std::cout << "[ERROR]: math.Min must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Max") {
                                    if (argc == 2) {
                                        registerType = context.SetIntRegister(operatorEvaluation, std::max<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1])));
                                        operatorEvaluation = OperatorEvaluation::NONE;
                                    } else {
                                        std::cout << "[ERROR]: math.Max must have 2 arguments, this call has " << argc << "." << std::endl;
                                    }
                                } else if (method == "Clamp") {
                                    if (argc == 3) {
                                        registerType = context.SetIntRegister(operatorEvaluation, std::clamp<int32_t>(ParseIntExpression(context, args[0]), ParseIntExpression(context, args[1]), ParseIntExpression(context, args[2])));
                                        operatorEvaluation = OperatorEvaluation::NONE;
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

        return result;
    }
};

int main(int argc, char** argv) {
    //std::string testString = "A, bcde, efef, hpphuweaf, aew";
    //std::vector<std::string> split = std::vector<std::string>();
    //size_t size = SplitString(testString, ",", split);
    //std::cout << "Size: " << size << std::endl;

    std::string test = "{\nif (player.HasPerk(ARCANE_EYES)) {\nconsole.Print(\"There is a mana fountain in the area, but you can't benefit from it.\\n\");\n} else {\nconsole.Print(\"Nothing of interest can be found in the room.\\n\");\n} end;\n}";
    Interpreter interp = Interpreter();
    interp.ParseStatement(test);

    return 0;
}