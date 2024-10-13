#include "scanner.hpp"
#include <cctype>
#include <cstddef>
#include <string_view>

using namespace Scanners;

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
    token.type = TokenType::ERROR;
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
    return makeToken(TokenType::STRING);
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

    return makeToken(TokenType::NUMBER);
}

TokenType checkKeyword(size_t start, size_t remLen, std::string_view remWord, TokenType type) {
    std::string_view keyword{scanner.start + start, remLen};

    if (scanner.current - scanner.start == start + remLen && keyword == remWord) {
        return type;
    }

    return TokenType::IDENTIFIER;
}

TokenType identType() {
    switch (scanner.start[0]) {
        case 'a':
            return checkKeyword(1, 2, "nd", TokenType::AND);
        case 'c':
            return checkKeyword(1, 4, "lass", TokenType::CLASS);
        case 'e':
            return checkKeyword(1, 3, "lse", TokenType::ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a':
                        return checkKeyword(2, 3, "lse", TokenType::FALSE);
                    case 'o':
                        return checkKeyword(2, 1, "r", TokenType::FOR);
                    case 'u':
                        return checkKeyword(2, 1, "n", TokenType::FUN);
                }
            }
            break;
        case 'i':
            return checkKeyword(1, 1, "f", TokenType::IF);
        case 'n':
            return checkKeyword(1, 2, "il", TokenType::NIL);
        case 'o':
            return checkKeyword(1, 1, "r", TokenType::OR);
        case 'p':
            return checkKeyword(1, 4, "rint", TokenType::PRINT);
        case 'r':
            return checkKeyword(1, 5, "eturn", TokenType::RETURN);
        case 's':
            return checkKeyword(1, 4, "uper", TokenType::SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h':
                        return checkKeyword(2, 2, "is", TokenType::THIS);
                    case 'r':
                        return checkKeyword(2, 2, "ue", TokenType::TRUE);
                }
            }
            break;
        case 'v':
            return checkKeyword(1, 2, "ar", TokenType::VAR);
        case 'w':
            return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }

    return TokenType::IDENTIFIER;
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
        return makeToken(TokenType::TOK_EOF);
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
            return makeToken(TokenType::LEFT_PAREN);
        case ')':
            return makeToken(TokenType::RIGHT_PAREN);
        case '{':
            return makeToken(TokenType::LEFT_BRACE);
        case '}':
            return makeToken(TokenType::RIGHT_BRACE);
        case ';':
            return makeToken(TokenType::SEMICOLON);
        case ',':
            return makeToken(TokenType::COMMA);
        case '.':
            return makeToken(TokenType::DOT);
        case '-':
            return makeToken(TokenType::MINUS);
        case '+':
            return makeToken(TokenType::PLUS);
        case '/':
            return makeToken(TokenType::SLASH);
        case '*':
            return makeToken(TokenType::STAR);
        case '!':
            return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=':
            return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<':
            return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>':
            return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"':
            return string();
    }

    return errorToken("Unexpected character");
}
