#pragma once

#include <vector>
#include "value.hpp"

enum OpCode : uint8_t {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
};

struct Chunk {
    ValueArray constants;
    std::vector<uint8_t> code;
    std::vector<int> lines;

    Chunk() : constants(), code(), lines() {}
    ~Chunk() { freeChunk(); }

    constexpr std::size_t count() const { return code.size(); }
    constexpr std::size_t capacity() const { return code.capacity(); }

    void writeChunk(uint8_t byte, int line);
    void freeChunk();
    void freeLines();
    int addConstant(Value value);
};
