#include "scanner.hpp"
#include <cctype>
#include <cstddef>
#include <string_view>

using namespace Scanners;

void initScanner(std::string_view source) {
  scanner = Scanner{source};
}

bool isAtEnd() { return *scanner.current == '\0'; }

Token makeToken(TokenType type) {
    Token token{};
    token.type = type;
    token.start = scanner.start;
    token.length = static_cast<int>(scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

Token errorToken(std::string_view msg) {
    Token token{};
    token.type = TokenType::err;
    token.start = msg.data();
    token.length = static_cast<int>(msg.length());
    token.line = scanner.line;

    return token;
}

char advance() {
    scanner.current++;
    return scanner.current[-1];
}

bool match(char token) {
    if (isAtEnd()) {
        return false;
    }

    if (*scanner.current != token) {
        return false;
    }

    scanner.current++;
    return true;
}

char peek() { return *scanner.current; }
char peekNext() {
    if (isAtEnd()) {
        return '\0';
    }

    return scanner.current[1];
}

void skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd())
                        advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            scanner.line++;
        }

        advance();
    }

    if (isAtEnd()) {
        return errorToken("Unterminated string");
    }

    advance();
    return makeToken(TokenType::string);
}

Token number() {
    while (std::isdigit(peek())) {
        advance();
    }

    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) {
            advance();
        }
    }

    return makeToken(TokenType::number);
}

TokenType checkKeyword(size_t start, size_t remLen, std::string_view remWord, TokenType type) {
    std::string_view keyword{scanner.start + start, remLen};

    if (static_cast<size_t>(scanner.current - scanner.start) == start + remLen && keyword == remWord) {
        return type;
    }

    return TokenType::identifier;
}

TokenType identType() {
    switch (scanner.start[0]) {
        case 'a':
            return checkKeyword(1, 2, "nd", TokenType::tok_and);
        case 'c':
            return checkKeyword(1, 4, "lass", TokenType::tok_class);
        case 'e':
            return checkKeyword(1, 3, "lse", TokenType::tok_else);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':
                        return checkKeyword(2, 3, "lse", TokenType::tok_false);
                    case 'o':
                        return checkKeyword(2, 1, "r", TokenType::tok_for);
                    case 'u':
                        return checkKeyword(2, 1, "n", TokenType::fun);
                }
            }
            break;
        case 'i':
            return checkKeyword(1, 1, "f", TokenType::tok_if);
        case 'n':
            return checkKeyword(1, 2, "il", TokenType::nil);
        case 'o':
            return checkKeyword(1, 1, "r", TokenType::tok_or);
        case 'p':
            return checkKeyword(1, 4, "rint", TokenType::tok_print);
        case 'r':
            return checkKeyword(1, 5, "eturn", TokenType::tok_ret);
        case 's':
            return checkKeyword(1, 4, "uper", TokenType::super);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return checkKeyword(2, 2, "is", TokenType::tok_this);
                    case 'r':
                        return checkKeyword(2, 2, "ue", TokenType::tok_true);
                }
            }
            break;
        case 'v':
            return checkKeyword(1, 2, "ar", TokenType::var);
        case 'w':
            return checkKeyword(1, 4, "hile", TokenType::tok_while);
    }

    return TokenType::identifier;
}

Token identifier() {
    while (std::isalpha(peek()) || std::isdigit(peek())) {
        advance();
    }

    return makeToken(identType());
}

Token scanToken() {
    skipWhitespace();
    scanner.start = scanner.current;
    if (isAtEnd()) {
        return makeToken(TokenType::eof);
    }

    char c = advance();
    if (std::isalpha(c) || c == '-') {
        return identifier();
    }

    if (std::isdigit(c)) {
        return number();
    }

    switch (c) {
        case '(':
            return makeToken(TokenType::left_paren);
        case ')':
            return makeToken(TokenType::right_paren);
        case '{':
            return makeToken(TokenType::left_brace);
        case '}':
            return makeToken(TokenType::right_brace);
        case ';':
            return makeToken(TokenType::semicolon);
        case ',':
            return makeToken(TokenType::comma);
        case '.':
            return makeToken(TokenType::dot);
        case '-':
            return makeToken(TokenType::minus);
        case '+':
            return makeToken(TokenType::plus);
        case '/':
            return makeToken(TokenType::slash);
        case '*':
            return makeToken(TokenType::star);
        case '!':
            return makeToken(match('=') ? TokenType::bang_equal : TokenType::bang);
        case '=':
            return makeToken(match('=') ? TokenType::equal_equal : TokenType::equal);
        case '<':
            return makeToken(match('=') ? TokenType::less_equal : TokenType::less);
        case '>':
            return makeToken(match('=') ? TokenType::greater_equal : TokenType::greater);
        case '"':
            return string();
    }

    return errorToken("Unexpected character");
}
