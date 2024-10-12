#pragma once

#include "chunk.hpp"
#include "value.hpp"

constexpr int STACK_MAX = 256;

enum InterpretResult : uint8_t {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

struct VM {
    Chunk* chunk{nullptr};
    uint8_t* ip{nullptr};
    Value stack[STACK_MAX];
    Value* top;

    VM() = default;
    ~VM() = default;

    uint8_t readByte() { return *ip++; }
    Value readConstant() { return chunk->constants.values[readByte()]; }
    void resetStack() { this->top = this->stack; }
    void push(Value value);
    Value pop();

    InterpretResult interpret(Chunk* chunk);
    InterpretResult run();

    template<typename Op>
    void binaryOp(Op op) {
        Value b = pop();
        Value a = pop();
        push(op(a, b));
    }
};

namespace VmInstance {
    inline VM vmInstance{};
}

void initVM();
void freeVM();
InterpretResult interpret(std::string_view source);
