#include "compiler.hpp"
#include <charconv>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <string_view>
#include "chunk.hpp"
#include "scanner.hpp"

using namespace Parsers;
using namespace Chunks;

static void errAt(const Token& token, std::string_view msg) {
    if (parser.panicMode()) {
        return;
    }

    parser.setPanicMode(true);
    std::cerr << std::format("[line {}] Error", token.line);
    if (token.type == TokenType::TOK_EOF) {
        std::cerr << "At end";
    } else if (token.type != TokenType::ERROR) {
        std::cerr << std::format(" at '{}'", std::string_view(token.start, token.length));
    }

    std::cerr << std::format(": {}\n", msg);
    parser.setHadError(true);
}

static void error(std::string_view msg) { errAt(parser.getPrev(), msg); }

static void errAtCurrent(std::string_view msg) { errAt(parser.getCurrent(), msg); }

static void advance() {
    parser.setPrev(parser.getCurrent());
    while (true) {
        parser.setCurrent(scanToken());
        if (parser.getCurrent().type != TokenType::ERROR) {
            break;
        }

        errAtCurrent(parser.getCurrent().start);
    }
}

static void consume(TokenType type, std::string_view msg) {
    if (parser.getCurrent().type == type) {
        advance();
        return;
    }

    errAtCurrent(msg);
}

static void emitByte(uint8_t byte) { compilingChunk->writeChunk(byte, parser.getPrev().line); }

static void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

[[nodiscard]] static Chunk* currentChunk() { return compilingChunk.get(); }

static void emitReturn() { emitByte(static_cast<uint8_t>(OpCode::OP_RETURN)); }

static void endCompiler() { emitReturn(); }

static void number() {
    double value{};
    auto [val, err] = std::from_chars(parser.getPrev().start, parser.getPrev().start + parser.getPrev().length, value);
}

static void expression() {}

bool compile(std::string_view source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = std::make_unique<Chunk>(*chunk);
    parser.setHadError(false);
    parser.setPanicMode(false);

    advance();
    expression();
    consume(TokenType::TOK_EOF, "Expect end of expression");
    compilingChunk.reset();
    endCompiler();

    return !parser.hadError();
}
