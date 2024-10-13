#pragma once

#include <array>
#include <string_view>
#include "chunk.hpp"
#include "value.hpp"

constexpr int STACK_MAX = 256;

enum class InterpretResult : uint8_t {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

struct VM {
    Chunk* chunk{nullptr};
    uint8_t* ip{nullptr};
    std::array<Value, STACK_MAX> stack{};
    Value* top{nullptr};

    constexpr VM() = default;
    constexpr ~VM() = default;

    [[nodiscard]] uint8_t readByte() { return *ip++; }
    [[nodiscard]] Value readConstant() { return chunk->constants.values[readByte()]; }
    void resetStack() { top = stack.data(); }
    void push(Value value);
    [[nodiscard]] Value pop();

    // [[nodiscard]] InterpretResult interpret(Chunk* chunk);
    [[nodiscard]] InterpretResult run();

    template<typename Op>
    void binaryOp(Op op) {
        Value b = pop();
        Value a = pop();
        push(op(a, b));
    }
};

namespace VmInstance {
    inline constinit VM vmInstance{};
} // namespace VmInstance

void initVM();
void freeVM();
[[nodiscard]] InterpretResult interpret(std::string_view source);
