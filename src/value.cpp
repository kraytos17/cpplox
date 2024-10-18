#include "value.hpp"
#include <print>
#include "forward_decl.hpp"
#include "inline_decl.hpp"

void ValueArray::writeValue(Value value) { values.push_back(value); }

void ValueArray::freeValueArray() {
    values.clear();
    values.shrink_to_fit();
}

void printValue(Value value) {
    switch (value.type) {
        case ValueType::val_bool:
            std::print("{}", asBool(value) ? "true" : "false");
            break;
        case ValueType::val_nil:
            std::print("nil");
            break;
        case ValueType::val_number:
            std::print("{:g}", asNumber(value));
            break;
        case ValueType::val_obj:
            printObj(value);
            break;
        default:
            return;
    }
}
