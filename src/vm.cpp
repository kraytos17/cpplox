#include "vm.hpp"
#include <functional>
#include "chunk.hpp"
#include "common.hpp"
#include "compiler.hpp"
#include "debug.hpp"
#include "value.hpp"

void initVM() {
    VmInstance::vmInstance.resetStack();
}

void freeVM() { }

InterpretResult VM::interpret(Chunk* chunk) {
    this->chunk = chunk;
    ip = chunk->code.data();
    return run();
}

InterpretResult VM::run() {
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        std::cout << "        ";
        for (const auto& value: std::span(stack, top)) {
            std::cout << "[ ";
            printValue(value);
            std::cout << " ]";
        }

        std::cout << '\n';
        disassembleInstruction(*this->chunk, static_cast<int>(ip - chunk->code.data()));
#endif
        uint8_t instruction = readByte();
        switch (instruction) {
            case OP_CONSTANT: {
                Value constant = readConstant();
                push(constant);
                break;
            }
            case OP_ADD:
                binaryOp(std::plus<>());
                break;
            case OP_SUBTRACT:
                binaryOp(std::minus<>());
                break;
            case OP_MULTIPLY:
                binaryOp(std::multiplies<>());
                break;
            case OP_DIVIDE:
                binaryOp(std::divides<>());
                break;
            case OP_NEGATE:
                printValue(-pop());
                break;
            case OP_RETURN:
                printValue(pop());
                std::cout << '\n';
                return INTERPRET_OK;
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
    compile(source);
    return INTERPRET_OK;
}
