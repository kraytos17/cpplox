#pragma once

#include <array>
#include <string_view>
#include "chunk.hpp"
#include "value.hpp"

constexpr int STACK_MAX = 256;

enum class InterpretResult : uint8_t {
    ok,
    compile_error,
    runtime_error,
};

struct VM {
    Chunk* chunk{nullptr};
    uint8_t* ip{nullptr};
    std::array<Value, STACK_MAX> stack{};
    Value* top{nullptr};
    Obj* objects{nullptr};

    constexpr VM() = default;
    constexpr ~VM() = default;

    [[nodiscard]] constexpr uint8_t readByte() { return *ip++; }
    [[nodiscard]] constexpr Value readConstant() { return chunk->constants.values[readByte()]; }
    constexpr void resetStack() { top = stack.data(); }
    constexpr void push(Value value);
    [[nodiscard]] constexpr Value pop();

    // [[nodiscard]] InterpretResult interpret(Chunk* chunk);
    [[nodiscard]] InterpretResult run();
};

namespace VmInstance {
    inline constinit VM vm{};
} // namespace VmInstance

[[nodiscard]] InterpretResult interpret(std::string_view source);

constexpr void initVM() { VmInstance::vm.resetStack(); }

constexpr void freeVM() {}
