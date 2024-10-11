#include "chunk.hpp"

void Chunk::writeChunk(uint8_t byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

void Chunk::freeChunk() {
    code.clear();
    code.shrink_to_fit();
    freeLines();
    constants.freeValueArray();
}

int Chunk::addConstant(Value value) {
    constants.writeValue(value);
    return constants.count() - 1;
}

void Chunk::freeLines() {
    lines.clear();
    lines.shrink_to_fit();
}
