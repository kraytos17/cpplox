#pragma once

#include <cassert>
#include <variant>
#include <vector>
#include "forward_decl.hpp"

struct Value {
    ValueType type{};
    std::variant<std::monostate, bool, double, Obj*> as{};

    constexpr Value(bool b) noexcept : type(ValueType::val_bool), as(b) {}
    constexpr Value(double n) noexcept : type(ValueType::val_number), as(n) {}
    constexpr Value() noexcept : type(ValueType::val_nil), as(std::monostate{}) {}
    constexpr Value(Obj* o) noexcept : type(ValueType::val_obj), as(o) {}
};

bool isBool(const Value& value) noexcept;
bool isNil(const Value& value) noexcept;
bool isNumber(const Value& value) noexcept;
Value boolValue(bool value);
Value nilValue();
Value numberValue(double value);
Value objValue(Obj* obj);

bool asBool(const Value& value);
double asNumber(const Value& value);

struct ValueArray {
    std::vector<Value> values;

    ValueArray() noexcept;
    ~ValueArray() noexcept;

    std::size_t count() const noexcept;
    std::size_t capacity() const noexcept;

    void writeValue(Value value);
    void freeValueArray();
};

inline bool valuesEq(Value a, Value b);
void printValue(Value value);
