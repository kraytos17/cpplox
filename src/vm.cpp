#include "vm.hpp"
#include <memory>
#include "common.hpp"
#include "debug.hpp"
#include "value.hpp"

static std::unique_ptr<VM> vmInstance;

void VM::initVM() {
    vmInstance = std::make_unique<VM>();
    resetStack();
}

void VM::freeVM() { vmInstance.reset(); }

InterpretResult VM::interpret(Chunk* chunk) {
    this->chunk = chunk;
    ip = chunk->code.data();
    return run();
}

InterpretResult VM::run() {
    while (true) {
#ifdef DEBUG_TRACE_EXECUTION
        std::cout << "        ";
        for (const auto& value: std::span(this->stack, this->top)) {
            std::cout << "[ ";
            printValue(value);
            std::cout << " ]";
        }

        std::cout << '\n';
        disassembleInstruction(*this->chunk, static_cast<int>(this->ip - this->chunk->code.data()));
#endif
        uint8_t instruction = readByte();
        switch (instruction) {
            case OpCode::OP_CONSTANT: {
                Value constant = readConstant();
                push(constant);
                break;
            }
            case OpCode::OP_RETURN:
                printValue(pop());
                std::cout << '\n';
                return INTERPRET_OK;
        }
    }
}

void VM::push(Value value) {
    *this->top = value;
    this->top++;
}

Value VM::pop() {
    this->top--;
    return *this->top;
}
