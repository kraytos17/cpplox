#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include "chunk.hpp"
#include "scanner.hpp"

enum class Precedence : uint8_t {
    none,
    assignment,
    prec_or,
    prec_and,
    equality,
    comparison,
    term,
    factor,
    unary,
    call,
    primary,
};

using ParseFn = std::function<void()>;

struct ParseRule {
    std::optional<ParseFn> prefix{};
    std::optional<ParseFn> infix{};
    Precedence precedence{};
};

class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    Token getCurrent() const { return current; }
    void setCurrent(const Token& token) { current = token; }

    Token getPrev() const { return prev; }
    void setPrev(const Token& token) { prev = token; }

    bool hadError() const { return hadErrorFlag; }
    void setHadError(bool flag) { hadErrorFlag = flag; }

    bool panicMode() const { return panicModeFlag; }
    void setPanicMode(bool flag) { panicModeFlag = flag; }

private:
    Token current{};
    Token prev{};
    bool hadErrorFlag{};
    bool panicModeFlag{};
};

namespace Parsers {
    inline constinit Parser parser{};
}

namespace Chunks {
    inline constinit std::unique_ptr<Chunk> compilingChunk{nullptr};
}

bool compile(std::string_view source, Chunk* chunk);
