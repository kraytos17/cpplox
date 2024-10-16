#include "value.hpp"
#include <format>
#include <iostream>
#include "inline_decl.hpp"

void ValueArray::writeValue(Value value) { values.push_back(value); }

void ValueArray::freeValueArray() {
    values.clear();
    values.shrink_to_fit();
}

void printValue(Value value) {
    switch (value.type) {
        case ValueType::val_bool:
            std::cout << std::format("{}", asBool(value) ? "true" : "false");
            break;
        case ValueType::val_nil:
            std::cout << "nil";
            break;
        case ValueType::val_number:
            std::cout << std::format("{:g}", asNumber(value));
            break;
        default:
            return;
    }
}
