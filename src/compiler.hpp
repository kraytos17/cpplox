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
    Token getCurrent() const { return m_current; }
    void setCurrent(const Token& token) { m_current = token; }

    Token getPrev() const { return m_prev; }
    void setPrev(const Token& token) { m_prev = token; }

    bool hadError() const { return m_hadErrorFlag; }
    void setHadError(bool flag) { m_hadErrorFlag = flag; }

    bool panicMode() const { return m_panicModeFlag; }
    void setPanicMode(bool flag) { m_panicModeFlag = flag; }

private:
    Token m_current{};
    Token m_prev{};
    bool m_hadErrorFlag{};
    bool m_panicModeFlag{};
};

namespace Parsers {
    inline constinit Parser parser{};
}

namespace Chunks {
    inline constinit std::unique_ptr<Chunk> compilingChunk{nullptr};
}

bool compile(std::string_view source, Chunk* chunk);
