#include <ArsTimoris/Sockscript/InterpreterContext.h>
#include <iostream>
#include <format>

std::string ReaderModeDisplayName(ReaderMode a_mode) {
    switch (a_mode) {
        case ReaderMode::READ:
            return "READING";
        case ReaderMode::VAR_NAME:
            return "VAR_NAM";
        case ReaderMode::VAR_TYPE:
            return "VAR_TYP";
        case ReaderMode::VAR_VALUE:
            return "VAR_VAL";
        case ReaderMode::VAR_EXPR:
            return "VAR_EXP";
        case ReaderMode::RET_EXPR:
            return "RET_EXP";
        case ReaderMode::OBJECT:
            return "OBJECTS";
        case ReaderMode::EXPRESSION:
            return "EXPRESS";
        case ReaderMode::ARGS:
            return "ARGUMEN";
        case ReaderMode::FALLTHROUGH:
            return "FALLTHR";
        case ReaderMode::FINISHED:
            return "FINISHE";
        default:
            return "INVALID";
    }
}

std::string OperatorEvaluationDisplayName(OperatorEvaluation a_operator) {
    switch (a_operator) {
        case OperatorEvaluation::NONE:
            return "NON";
        case OperatorEvaluation::ADDI:
            return "ADD";
        case OperatorEvaluation::SUBT:
            return "SUB";
        case OperatorEvaluation::MULT:
            return "MUL";
        case OperatorEvaluation::DIVI:
            return "DIV";
        case OperatorEvaluation::MODU:
            return "MOD";
        case OperatorEvaluation::NOT:
            return "NOT";
        case OperatorEvaluation::AND:
            return "AND";
        case OperatorEvaluation::OR:
            return "IOR";
        case OperatorEvaluation::GT:
            return "GRT";
        case OperatorEvaluation::GTE:
            return "GTE";
        case OperatorEvaluation::LT:
            return "LST";
        case OperatorEvaluation::LTE:
            return "LTE";
        case OperatorEvaluation::EQ:
            return "EQU";
        case OperatorEvaluation::NEQ:
            return "NEQ";
        default:
            return "IVD";
    }
}

void InterpreterContext::DumpStringRegister(void) {
    for (size_t i = 0; i < stringRegisters.size(); ++i) {
        std::cout << std::format("{}String Register {:0>2}: {}\x1b[39m", i == stringRegister ? "\x1b[32m" : "\x1b[34m", i + 1, stringRegisters[i]) << std::endl;
    }
}

void InterpreterContext::DumpIntRegister(void) {
    for (size_t i = 0; i < intRegisters.size(); ++i) {
        std::cout << (i == intRegister ? "\x1b[32m" : "\x1b[34m") << "Int Register " << (i + 1) << ": " << intRegisters[i] << "\x1b[39m" << std::endl;
    }
}

void InterpreterContext::DumpFloatRegister(void) {
    for (size_t i = 0; i < floatRegisters.size(); ++i) {
        std::cout << (i == floatRegister ? "\x1b[32m" : "\x1b[34m") << "Float Register " << (i + 1) << ": " << floatRegisters[i] << "\x1b[39m" << std::endl;
    }
}

void InterpreterContext::DumpBoolRegister(void) {
    for (size_t i = 0; i < boolRegisters.size(); ++i) {
        std::cout << (i == boolRegister ? "\x1b[32m" : "\x1b[34m") << "Bool Register " << (i + 1) << ": " << (boolRegisters[i] ? "true" : "false") << "\x1b[39m" << std::endl;
    }
}

RegisterType InterpreterContext::SetIntRegister(int32_t a_value) {
    switch (operatorEvaluation) {
        case OperatorEvaluation::NONE:
            intRegisters[++intRegister] = a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::ADDI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] + a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = floatRegisters[floatRegister] + a_value;
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::SUBT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] - a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = floatRegisters[floatRegister] - a_value;
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::MULT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] * a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = floatRegisters[floatRegister] * a_value;
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::DIVI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] / a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = floatRegisters[floatRegister] / a_value;
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::MODU:
            intRegisters[intRegister] = intRegisters[intRegister] % a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::GT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] > a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] > a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::GTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] >= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::LT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] < a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] < a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::LTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] <= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::EQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] == a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] == a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::NEQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] != a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] != a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        default:
            lastRegister = RegisterType::ERROR;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::ERROR;
    }
    
}

RegisterType InterpreterContext::SetFloatRegister(float a_value) {
    switch (operatorEvaluation) {
        case OperatorEvaluation::NONE:
            floatRegisters[++floatRegister] = a_value;
            lastRegister = RegisterType::FLOAT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::FLOAT;
        case OperatorEvaluation::ADDI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] + a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] + a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::SUBT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] - a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] - a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::MULT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] * a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] * a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::DIVI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] / a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] / a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::GT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] > a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] > a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::GTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] >= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::LT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] < a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] < a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::LTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] <= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::EQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] == a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] == a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        case OperatorEvaluation::NEQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] != a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] != a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR:
                    lastRegister = RegisterType::ERROR;
                    return RegisterType::ERROR;
            }
        default:
            lastRegister = RegisterType::ERROR;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::ERROR;
    }
    
}

RegisterType InterpreterContext::SetBoolRegister(bool a_value) {
    switch (operatorEvaluation) {
        case OperatorEvaluation::NONE:
            boolRegisters[++boolRegister] = a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::NOT:
            boolRegisters[++boolRegister] = !a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::AND:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] && a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::OR:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] || a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::EQ:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] == a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::NEQ:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] != a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        default:
            lastRegister = RegisterType::ERROR;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::ERROR;
    }
}