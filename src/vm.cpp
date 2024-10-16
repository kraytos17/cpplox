#include "vm.hpp"
#include <format>
#include <functional>
#include <iostream>
#include <span>
#include <string_view>
#include "chunk.hpp"
#include "common.hpp"
#include "compiler.hpp"
#include "debug.hpp"
#include "inline_decl.hpp"

using namespace VmInstance;

void initVM() { vmInstance.resetStack(); }
void freeVM() {}

// InterpretResult VM::interpret(Chunk* chunk) {
//     this->chunk = chunk;
//     ip = chunk->code.data();
//     return run();
// }

static void runtimeError(const std::string& message) {
    std::string errorMessage = std::format("Runtime Error: {}", message);
    std::cerr << errorMessage << "\n";

    size_t instruction = vmInstance.ip - vmInstance.chunk->code.data() - 1;
    int line = vmInstance.chunk->lines[instruction];

    std::cerr << std::format("[line {}] in script\n", line);
    vmInstance.resetStack();
}

template<typename... Args>
static void formatRuntimeError(const std::string& fmt, Args&&... args) {
    std::string formattedMessage = std::format(fmt, std::forward<Args>(args)...);
    runtimeError(formattedMessage);
}

static Value peek(int distance) { return vmInstance.top[-1 - distance]; }

void VM::push(Value value) {
    *top = value;
    top++;
}

Value VM::pop() {
    top--;
    return *top;
}

template<typename Op>
void binaryOp(Op op) {
    if (!isNumber(peek(0)) || !isNumber(peek(1))) {
        runtimeError("Operands must be numbers.");
    }

    auto b = asNumber(vmInstance.pop());
    auto a = asNumber(vmInstance.pop());
    vmInstance.push(numberValue(op(a, b)));
}

static bool isFalsey(const Value& value) { return isNil(value) || (isBool(value) && !asBool(value)); }

InterpretResult VM::run() {
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        std::cout << "        ";
        for (const auto& value: std::span(stack.data(), top)) {
            std::cout << "[ ";
            printValue(value);
            std::cout << " ]";
        }

        std::cout << '\n';
        disassembleInstruction(*this->chunk, static_cast<int>(ip - chunk->code.data()));
#endif
        uint8_t instruction = readByte();
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::constant: {
                Value constant = readConstant();
                push(constant);
                break;
            }
            case OpCode::nil:
                push(nilValue());
                break;
            case OpCode::op_true:
                push(boolValue(true));
                break;
            case OpCode::op_false:
                push(boolValue(false));
                break;
            case OpCode::equal: {
                auto b = pop();
                auto a = pop();
                push(boolValue(valuesEq(a, b)));
                break;
            }
            case OpCode::add:
                binaryOp(std::plus<>());
                break;
            case OpCode::subtract:
                binaryOp(std::minus<>());
                break;
            case OpCode::multiply:
                binaryOp(std::multiplies<>());
                break;
            case OpCode::divide:
                binaryOp(std::divides<>());
                break;
            case OpCode::op_not:
                push(boolValue(isFalsey(pop())));
                break;
            case OpCode::negate: {
                if (!isNumber(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return InterpretResult::runtime_error;
                }
                double value = asNumber(pop());
                push(numberValue(-value));
                break;
            }
            case OpCode::ret:
                printValue(pop());
                std::cout << '\n';
                return InterpretResult::ok;
            default:
                return InterpretResult::compile_error;
        }
    }
}

InterpretResult interpret(std::string_view source) {
    Chunk chunk{};

    if (!compile(source, &chunk)) {
        return InterpretResult::compile_error;
    }

    vmInstance.chunk = &chunk;
    vmInstance.ip = vmInstance.chunk->code.data();
    InterpretResult res = vmInstance.run();

    return res;
}
