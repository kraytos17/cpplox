#include "value.hpp"
#include <format>
#include <iostream>
#include "object.hpp"

bool isObj(const Value& value) noexcept { return value.type == ValueType::val_obj; }
Obj* asObj(const Value& value) {
    assert(isObj(value) && "Value is not an object.");
    return std::get<Obj*>(value.as);
}

bool isBool(const Value& value) noexcept { return value.type == ValueType::val_bool; }
bool isNil(const Value& value) noexcept { return value.type == ValueType::val_nil; }
bool isNumber(const Value& value) noexcept { return value.type == ValueType::val_number; }
Value boolValue(bool value) { return Value(value); }
Value nilValue() { return Value{}; }
Value numberValue(double value) { return Value(value); }
Value objValue(Obj* obj) { return Value(obj); }

bool asBool(const Value& value) {
    assert(isBool(value) && "Value is not a boolean.");
    return std::get<bool>(value.as);
}

double asNumber(const Value& value) {
    assert(isNumber(value) && "Value is not a number.");
    return std::get<double>(value.as);
}

ValueArray::ValueArray() noexcept : values() {}
ValueArray::~ValueArray() noexcept { freeValueArray(); }

std::size_t ValueArray::count() const noexcept { return values.size(); }
std::size_t ValueArray::capacity() const noexcept { return values.capacity(); }

void ValueArray::writeValue(Value value) { values.push_back(value); }

void ValueArray::freeValueArray() {
    values.clear();
    values.shrink_to_fit();
}

bool valuesEq(Value a, Value b) {
    if (a.type != b.type)
        return false;
    switch (a.type) {
        case ValueType::val_bool:
            return asBool(a) == asBool(b);
        case ValueType::val_nil:
            return true;
        case ValueType::val_number:
            return asNumber(a) == asNumber(b);
        case ValueType::val_obj:
            return asObj(a) == asObj(b);
        default:
            return false;
    }
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
