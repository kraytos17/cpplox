#include "vm.hpp"
#include <functional>
#include <span>
#include "chunk.hpp"
#include "common.hpp"
#include "compiler.hpp"
#include "debug.hpp"
#include "value.hpp"

using namespace VmInstance;

void initVM() { vmInstance.resetStack(); }

void freeVM() {}

// InterpretResult VM::interpret(Chunk* chunk) {
//     this->chunk = chunk;
//     ip = chunk->code.data();
//     return run();
// }

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
            case OpCode::OP_CONSTANT: {
                Value constant = readConstant();
                push(constant);
                break;
            }
            case OpCode::OP_ADD:
                binaryOp(std::plus<>());
                break;
            case OpCode::OP_SUBTRACT:
                binaryOp(std::minus<>());
                break;
            case OpCode::OP_MULTIPLY:
                binaryOp(std::multiplies<>());
                break;
            case OpCode::OP_DIVIDE:
                binaryOp(std::divides<>());
                break;
            case OpCode::OP_NEGATE:
                push(-pop());
                break;
            case OpCode::OP_RETURN:
                printValue(pop());
                std::cout << '\n';
                return InterpretResult::INTERPRET_OK;
        }
    }
}

void VM::push(Value value) {
    *top = value;
    top++;
}

Value VM::pop() {
    top--;
    return *top;
}

InterpretResult interpret(std::string_view source) {
    Chunk chunk{};

    if (!compile(source, &chunk)) {
        return InterpretResult::INTERPRET_COMPILE_ERROR;
    }

    vmInstance.chunk = &chunk;
    vmInstance.ip = vmInstance.chunk->code.data();
    InterpretResult res = vmInstance.run();

    return res;
}
