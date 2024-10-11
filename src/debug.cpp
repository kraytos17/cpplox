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

static int simpleInstruction(std::string_view name, int offset) {
    std::cout << name << '\n';
    return offset + 1;
}

static int constantInstruction(std::string_view name, const Chunk& chunk, int offset) {
    uint8_t constant = chunk.code[offset + 1];
    std::cout << std::format("{:<10} {:4} '", name, constant);
    printValue(chunk.constants.values[constant]);
    std::cout << "'\n";

    return offset + 2;
}

int disassembleInstruction(const Chunk& chunk, int offset) {
    std::cout << std::format("{:04} ", offset);
    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        std::cout << "   | ";
    } else {
        std::cout << std::format("{:4d} ", chunk.lines[offset]);
    }

    uint8_t instruction = chunk.code[offset];
    switch (instruction) {
        case OpCode::OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
        case OpCode::OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            std::cout << std::format("Unknown opcode {}\n", instruction);
            return offset + 1;
    }
}
