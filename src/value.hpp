#pragma once

#include <cstddef>
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

struct ValueArray {
    std::vector<Value> values{};

    ValueArray() noexcept : values(){}
    ValueArray(const ValueArray& other) = default;
    ~ValueArray() noexcept { freeValueArray(); }

    constexpr size_t count() const noexcept { return values.size(); }
    constexpr size_t capacity() const noexcept { return values.capacity(); }

    void writeValue(Value value);
    void freeValueArray();
};

void printValue(Value value);
