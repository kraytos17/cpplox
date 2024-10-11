#pragma once

#include <format>
#include <iostream>
#include <vector>

using Value = double;

struct ValueArray {
    std::vector<Value> values;

    ValueArray() : values() {}
    ~ValueArray() { freeValueArray(); }

    constexpr std::size_t count() const { return values.size(); }
    constexpr std::size_t capacity() const { return values.capacity(); }

    void writeValue(Value value);
    void freeValueArray();
};

inline void printValue(Value value) { std::cout << std::format("{:g}", value); }
