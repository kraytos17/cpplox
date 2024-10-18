#include "vm.hpp"
#include <cstring>
#include <format>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include "chunk.hpp"
#include "common.hpp"
#include "compiler.hpp"
#include "debug.hpp"
#include "inline_decl.hpp"

using namespace VmInstance;

// InterpretResult VM::interpret(Chunk* chunk) {
//     this->chunk = chunk;
//     ip = chunk->code.data();
//     return run();
// }

static void runtimeError(const std::string& message) {
    std::string errorMessage = std::format("Runtime Error: {}", message);
    std::cerr << errorMessage << "\n";

    size_t instruction = vm.ip - vm.chunk->code.data() - 1;
    int line = vm.chunk->lines[instruction];

    std::cerr << std::format("[line {}] in script\n", line);
    vm.resetStack();
}

template<typename... Args>
static void formatRuntimeError(std::string_view fmt, Args&&... args) {
    std::string formattedMessage = std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
    runtimeError(formattedMessage);
}


static Value peek(int distance) { return vm.top[-1 - distance]; }
static void concatenate() {
    auto b = asObjString(vm.pop());
    auto a = asObjString(vm.pop());

    auto len = a->getLength() + b->getLength();
    auto chars = new char[len + 1];
    std::memcpy(chars, a->getChars().data(), a->getLength());
    std::memcpy(chars + a->getLength(), b->getChars().data(), b->getLength());
    chars[len] = '\0';

    auto res = ObjString(std::string_view(chars, len));
    vm.push(objValue(&res));
}

constexpr void VM::push(Value value) {
    *top = value;
    top++;
}

constexpr Value VM::pop() {
    top--;
    return *top;
}

template<typename Op>
void binaryOp(Op op) {
    if (!isNumber(peek(0)) || !isNumber(peek(1))) {
        formatRuntimeError("Operands must be numbers.");
    }

    auto b = asNumber(vm.pop());
    auto a = asNumber(vm.pop());
    vm.push(numberValue(op(a, b)));
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
            case OpCode::greater:
                binaryOp(std::greater<>());
                break;
            case OpCode::less:
                binaryOp(std::less<>());
                break;
            case OpCode::add: {
                if (isObjString(peek(0)) && isObjString(peek(1))) {
                    concatenate();
                } else if (isNumber(peek(0)) && isNumber(peek(1))) {
                    auto b = asNumber(pop());
                    auto a = asNumber(pop());
                    push(numberValue(a + b));
                } else {
                    formatRuntimeError("Operands must be two numbers or two strings");
                    return InterpretResult::runtime_error;
                }
                break;
            }
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
                    formatRuntimeError("Operand must be a number.");
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

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code.data();
    InterpretResult res = vm.run();

    return res;
}
