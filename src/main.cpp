#include <iostream>
#include "chunk.hpp"
#include "debug.hpp"
#include "vm.hpp"

int main() {
    VM vm;
    Chunk chunk{};
    int constant{chunk.addConstant(1.2)};
    chunk.writeChunk(OpCode::OP_CONSTANT, 123);
    chunk.writeChunk(constant, 123);
    chunk.writeChunk(OpCode::OP_RETURN, 123);
    disassembleChunk(chunk, "test_chunk");
    chunk.freeChunk();
    return 0;
}
