#pragma once

#include <vector>
#include "value.hpp"

enum class OpCode : uint8_t {
    constant,
    add,
    subtract,
    multiply,
    divide,
    negate,
    ret,
};

struct Chunk {
    ValueArray constants;
    std::vector<uint8_t> code;
    std::vector<int> lines;

    constexpr Chunk() : constants(), code(), lines() {}
    constexpr ~Chunk() { freeChunk(); }

    [[nodiscard]] constexpr std::size_t count() const { return code.size(); }
    [[nodiscard]] constexpr std::size_t capacity() const { return code.capacity(); }

    void writeChunk(uint8_t byte, int line);
    void freeChunk();
    void freeLines();
    int addConstant(Value value);
};
