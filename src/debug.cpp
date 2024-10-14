#include "debug.hpp"
#include <format>
#include <iostream>
#include "value.hpp"

void disassembleChunk(const Chunk& chunk, std::string_view name) {
    std::cout << std::format("== {} ==\n", name);
    for (std::size_t offset{0}; offset < chunk.count();) {
        offset = disassembleInstruction(chunk, offset);
    }
}

[[nodiscard]] static int simpleInstruction(std::string_view name, int offset) {
    std::cout << name << '\n';
    return offset + 1;
}

[[nodiscard]] static int constantInstruction(std::string_view name, const Chunk& chunk, int offset) {
    uint8_t constant = chunk.code[offset + 1];
    std::cout << std::format("{:<10} {:4} '", name, constant);
    printValue(chunk.constants.values[constant]);
    std::cout << "'\n";

    return offset + 2;
}

[[nodiscard]] int disassembleInstruction(const Chunk& chunk, int offset) {
    std::cout << std::format("{:04} ", offset);
    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        std::cout << "   | ";
    } else {
        std::cout << std::format("{:4d} ", chunk.lines[offset]);
    }

    uint8_t instruction = chunk.code[offset];
    switch (static_cast<OpCode>(instruction)) {
        case OpCode::constant:
            return constantInstruction("constant", chunk, offset);
        case OpCode::add:
            return simpleInstruction("add", offset);
        case OpCode::subtract:
            return simpleInstruction("subtract", offset);
        case OpCode::multiply:
            return simpleInstruction("multiply", offset);
        case OpCode::divide:
            return simpleInstruction("divide", offset);
        case OpCode::negate:
            return simpleInstruction("negate", offset);
        case OpCode::ret:
            return simpleInstruction("ret", offset);
        default:
            std::cout << std::format("Unknown opcode {}\n", instruction);
            return offset + 1;
    }
}
