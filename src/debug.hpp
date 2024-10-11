#pragma once

#include <string_view>
#include "chunk.hpp"

void disassembleChunk(const Chunk& chunk, std::string_view name);
int disassembleInstruction(const Chunk& chunk, int offset);
