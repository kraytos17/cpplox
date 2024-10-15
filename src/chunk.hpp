#pragma once

#include <vector>
#include "value.hpp"

enum class OpCode : uint8_t {
    constant,
    nil,
    op_true,
    op_false,
    equal,
    greater,
    less,
    add,
    subtract,
    multiply,
    divide,
    op_not,
    negate,
    ret,
};

struct Chunk {
    ValueArray constants;
    std::vector<uint8_t> code;
    std::vector<int> lines;

    constexpr Chunk() : constants(), code(), lines() {}
    constexpr ~Chunk() { freeChunk(); }

    [[nodiscard]] constexpr std::size_t count() const noexcept { return code.size(); }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return code.capacity(); }

    void writeChunk(uint8_t byte, int line);
    void freeChunk();
    void freeLines();
    int addConstant(Value value);
};
