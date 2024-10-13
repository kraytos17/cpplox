#pragma once

#include <cstdint>
#include <string_view>

enum class TokenType : uint8_t {
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    IDENTIFIER,
    STRING,
    NUMBER,
    AND,
    CLASS,
    ELSE,
    FALSE,
    FOR,
    FUN,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
    ERROR,
    TOK_EOF,
};

struct Scanner {
    const char* start{nullptr};
    const char* current{nullptr};
    int line{1};

    constexpr explicit Scanner(std::string_view s) : start{s.data()}, current{s.data()} {}
    constexpr ~Scanner() = default;
};

struct Token {
    TokenType type;
    const char* start{nullptr};
    int length{};
    int line{};

    constexpr Token() = default;
    constexpr ~Token() = default;
};

namespace Scanners {
    inline constinit Scanner scanner{""};
} // namespace Scanners

void initScanner(std::string_view source);
[[nodiscard]] Token scanToken();
