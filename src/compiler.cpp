#include "compiler.hpp"
#include <charconv>
#include <cstdint>
#include <format>
#include <limits>
#include <memory>
#include <print>
#include <string_view>
#include "chunk.hpp"
#include "common.hpp"
#include "inline_decl.hpp"
#include "scanner.hpp"

#ifdef DEBUG_PRINT_CODE
#include "debug.hpp"
#endif

using namespace Parsers;
using namespace Chunks;

static void expression();
static void parsePrecedence(Precedence precedence);
static const ParseRule* getRule(TokenType type);

template<typename E>
concept ErrorHandler = requires(E e, const Token& token, std::string_view msg) {
    { e(token, msg) } -> std::same_as<void>;
};

template<ErrorHandler F>
static void handleError(F&& errorFunc, const Token& token, std::string_view msg) {
    if (parser.panicMode())
        return;

    parser.setPanicMode(true);
    std::forward<F>(errorFunc)(token, msg);
    parser.setHadError(true);
}

static void errAt(const Token& token, std::string_view msg) {
    std::print(stderr, "[line {}] Error", token.line);
    if (token.type == TokenType::eof) {
        std::print(stderr, " at end");
    } else if (token.type != TokenType::err) {
        std::print(stderr, " at '{}'", std::string_view(token.start, token.length));
    }
    std::println(stderr, ": {}", msg);
}

static void error(std::string_view msg) { handleError(errAt, parser.getPrev(), msg); }
static void errAtCurrent(std::string_view msg) { handleError(errAt, parser.getCurrent(), msg); }

static void advance() {
    parser.setPrev(parser.getCurrent());
    while (true) {
        parser.setCurrent(scanToken());
        if (parser.getCurrent().type != TokenType::err) {
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
static void emitBytes(uint8_t byte1, uint8_t byte2) { (emitByte(byte1), emitByte(byte2)); }
static void emitReturn() { emitByte(static_cast<uint8_t>(OpCode::ret)); }

static void endCompiler() {
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError()) {
        disassembleChunk(*compilingChunk, "code");
    }
#endif
}

static uint8_t makeConstant(Value value) {
    int constant = compilingChunk->addConstant(value);
    if (constant > std::numeric_limits<uint8_t>::max()) {
        error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<uint8_t>(constant);
}

static void emitConstant(Value value) { emitBytes(static_cast<uint8_t>(OpCode::constant), makeConstant(value)); }
static void number() {
    double value;
    auto [ptr, ec] = std::from_chars(parser.getPrev().start, parser.getPrev().start + parser.getPrev().length, value);
    if (ec == std::errc()) {
        emitConstant(value);
    } else {
        error("Invalid number format.");
    }
}

static void string() {
    emitConstant(objValue(copyString(parser.getPrev().start + 1, parser.getPrev().length - 2).get()));
}

static void grouping() {
    expression();
    consume(TokenType::right_paren, "Expect ')' after expression.");
}

static void unary() {
    TokenType operatorType = parser.getPrev().type;
    parsePrecedence(Precedence::unary);
    switch (operatorType) {
        case TokenType::bang:
            emitByte(static_cast<uint8_t>(OpCode::op_not));
            break;
        case TokenType::minus:
            emitByte(static_cast<uint8_t>(OpCode::negate));
            break;
        default:
            return;
    }
}

static void binary() {
    TokenType operatorType = parser.getPrev().type;
    const ParseRule* rule = getRule(operatorType);
    parsePrecedence(static_cast<Precedence>(static_cast<int>(rule->precedence) + 1));

    switch (operatorType) {
        case TokenType::bang_equal:
            emitBytes(static_cast<uint8_t>(OpCode::equal), static_cast<uint8_t>(OpCode::op_not));
            break;
        case TokenType::equal_equal:
            emitByte(static_cast<uint8_t>(OpCode::equal));
            break;
        case TokenType::greater:
            emitByte(static_cast<uint8_t>(OpCode::greater));
            break;
        case TokenType::greater_equal:
            emitBytes(static_cast<uint8_t>(OpCode::less), static_cast<uint8_t>(OpCode::op_not));
            break;
        case TokenType::less:
            emitByte(static_cast<uint8_t>(OpCode::less));
            break;
        case TokenType::less_equal:
            emitBytes(static_cast<uint8_t>(OpCode::greater), static_cast<uint8_t>(OpCode::op_not));
            break;
        case TokenType::plus:
            emitByte(static_cast<uint8_t>(OpCode::add));
            break;
        case TokenType::minus:
            emitByte(static_cast<uint8_t>(OpCode::subtract));
            break;
        case TokenType::star:
            emitByte(static_cast<uint8_t>(OpCode::multiply));
            break;
        case TokenType::slash:
            emitByte(static_cast<uint8_t>(OpCode::divide));
            break;
        default:
            return;
    }
}

static void expression() { parsePrecedence(Precedence::assignment); }
static void parsePrecedence(Precedence precedence) {
    advance();
    const auto& rule = getRule(parser.getPrev().type);
    if (!rule->prefix.has_value()) {
        error("Expect expression.");
        return;
    }

    rule->prefix.value()();
    while (precedence <= getRule(parser.getCurrent().type)->precedence) {
        advance();
        const auto& prec_rule = getRule(parser.getPrev().type);
        if (prec_rule->infix.has_value()) {
            prec_rule->infix.value()();
        } else {
            break;
        }
    }
}

static void literal() {
    switch (parser.getPrev().type) {
        case TokenType::tok_false:
            emitByte(static_cast<uint8_t>(OpCode::op_false));
            break;
        case TokenType::nil:
            emitByte(static_cast<uint8_t>(OpCode::nil));
            break;
        case TokenType::tok_true:
            emitByte(static_cast<uint8_t>(OpCode::op_true));
            break;
        default:
            return;
    }
}

bool compile(std::string_view source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = std::make_unique<Chunk>(*chunk);

    parser.setHadError(false);
    parser.setPanicMode(false);

    advance();
    expression();
    consume(TokenType::eof, "Expect end of expression.");
    endCompiler();
    return !parser.hadError();
}

constexpr auto TokenTypeCount = static_cast<size_t>(TokenType::eof) + 1;
const std::array<ParseRule, TokenTypeCount> rules = {{
    {grouping, nullptr, Precedence::none}, // TOKEN_LEFT_PAREN
    {nullptr, nullptr, Precedence::none}, // TOKEN_RIGHT_PAREN
    {nullptr, nullptr, Precedence::none}, // TOKEN_LEFT_BRACE
    {nullptr, nullptr, Precedence::none}, // TOKEN_RIGHT_BRACE
    {nullptr, nullptr, Precedence::none}, // TOKEN_COMMA
    {nullptr, nullptr, Precedence::none}, // TOKEN_DOT
    {unary, binary, Precedence::term}, // TOKEN_MINUS
    {nullptr, binary, Precedence::term}, // TOKEN_PLUS
    {nullptr, nullptr, Precedence::none}, // TOKEN_SEMICOLON
    {nullptr, binary, Precedence::factor}, // TOKEN_SLASH
    {nullptr, binary, Precedence::factor}, // TOKEN_STAR
    {unary, nullptr, Precedence::none}, // TOKEN_BANG
    {nullptr, binary, Precedence::equality}, // TOKEN_BANG_EQUAL
    {nullptr, nullptr, Precedence::none}, // TOKEN_EQUAL
    {nullptr, binary, Precedence::equality}, // TOKEN_EQUAL_EQUAL
    {nullptr, binary, Precedence::comparison}, // TOKEN_GREATER
    {nullptr, binary, Precedence::comparison}, // TOKEN_GREATER_EQUAL
    {nullptr, binary, Precedence::comparison}, // TOKEN_LESS
    {nullptr, binary, Precedence::comparison}, // TOKEN_LESS_EQUAL
    {nullptr, nullptr, Precedence::none}, // TOKEN_IDENTIFIER
    {string, nullptr, Precedence::none}, // TOKEN_STRING
    {number, nullptr, Precedence::none}, // TOKEN_NUMBER
    {nullptr, nullptr, Precedence::none}, // TOKEN_AND
    {nullptr, nullptr, Precedence::none}, // TOKEN_CLASS
    {nullptr, nullptr, Precedence::none}, // TOKEN_ELSE
    {literal, nullptr, Precedence::none}, // TOKEN_FALSE
    {nullptr, nullptr, Precedence::none}, // TOKEN_FOR
    {nullptr, nullptr, Precedence::none}, // TOKEN_FUN
    {nullptr, nullptr, Precedence::none}, // TOKEN_IF
    {literal, nullptr, Precedence::none}, // TOKEN_NIL
    {nullptr, nullptr, Precedence::none}, // TOKEN_OR
    {nullptr, nullptr, Precedence::none}, // TOKEN_PRINT
    {nullptr, nullptr, Precedence::none}, // TOKEN_RETURN
    {nullptr, nullptr, Precedence::none}, // TOKEN_SUPER
    {nullptr, nullptr, Precedence::none}, // TOKEN_THIS
    {literal, nullptr, Precedence::none}, // TOKEN_TRUE
    {nullptr, nullptr, Precedence::none}, // TOKEN_VAR
    {nullptr, nullptr, Precedence::none}, // TOKEN_WHILE
    {nullptr, nullptr, Precedence::none}, // TOKEN_ERROR
    {nullptr, nullptr, Precedence::none}, // TOKEN_EOF
}};

static const ParseRule* getRule(TokenType type) { return &rules[static_cast<size_t>(type)]; }
