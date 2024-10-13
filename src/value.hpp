#pragma once

#include <format>
#include <iostream>
#include <vector>

using Value = double;

struct ValueArray {
    std::vector<Value> values;

    constexpr ValueArray() : values() {}
    constexpr ~ValueArray() { freeValueArray(); }

    [[nodiscard]] constexpr std::size_t count() const { return values.size(); }
    [[nodiscard]] constexpr std::size_t capacity() const { return values.capacity(); }

    void writeValue(Value value);
    void freeValueArray();
};

inline void printValue(Value value) { std::cout << std::format("{:g}", value); }
