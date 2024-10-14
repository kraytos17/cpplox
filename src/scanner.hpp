#pragma once

#include <cstdint>
#include <string_view>

enum class TokenType : uint8_t {
    left_paren,
    right_paren,
    left_brace,
    right_brace,
    comma,
    dot,
    minus,
    plus,
    semicolon,
    slash,
    star,
    bang,
    bang_equal,
    equal,
    equal_equal,
    greater,
    greater_equal,
    less,
    less_equal,
    identifier,
    string,
    number,
    tok_and,
    tok_class,
    tok_else,
    tok_false,
    tok_for,
    fun,
    tok_if,
    nil,
    tok_or,
    tok_print,
    tok_ret,
    super,
    tok_this,
    tok_true,
    var,
    tok_while,
    err,
    eof,
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
