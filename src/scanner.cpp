#include "scanner.hpp"
#include <cctype>
#include <string_view>

bool isAtEnd() { return *Scanners::scanner.current == '\0'; }

Token makeToken(TokenType type) {
    Token token{};
    token.type = type;
    token.start = Scanners::scanner.start;
    token.length = static_cast<int>(Scanners::scanner.current - Scanners::scanner.start);
    token.line = Scanners::scanner.line;

    return token;
}

Token errorToken(std::string_view msg) {
    Token token{};
    token.type = TOKEN_ERROR;
    token.start = msg.data();
    token.length = msg.length();
    token.line = Scanners::scanner.line;

    return token;
}

char advance() {
    Scanners::scanner.current++;
    return Scanners::scanner.current[-1];
}

bool match(char token) {
    if (isAtEnd())
        return false;

    if (*Scanners::scanner.current != token)
        return false;

    Scanners::scanner.current++;
    return true;
}

char peek() { return *Scanners::scanner.current; }
char peekNext() {
    if (isAtEnd())
        return '\0';

    return Scanners::scanner.current[1];
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
                Scanners::scanner.line++;
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
    while (peek() != '"' && isAtEnd()) {
        if (peek() == '\n')
            Scanners::scanner.line++;

        advance();
    }

    if (isAtEnd())
        return errorToken("Unterminated string");

    advance();
    return makeToken(TOKEN_STRING);
}

Token number() {
    while (std::isdigit(peek()))
        advance();

    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek()))
            advance();
    }

    return makeToken(TOKEN_NUMBER);
}

TokenType identType() {
    switch (Scanners::scanner.start[0]) {}

    return TOKEN_IDENTIFIER;
}


Token identifier() {
    while (std::isalpha(peek()) || std::isdigit(peek()))
        advance();

    return makeToken(identType());
}

Token scanToken() {
    skipWhitespace();
    Scanners::scanner.start = Scanners::scanner.current;
    if (isAtEnd())
        return makeToken(TOKEN_EOF);

    char c = advance();
    if (std::isalpha(c) || c == '-')
        return identifier();

    if (std::isdigit(c))
        return number();

    switch (c) {
        case '(':
            return makeToken(TOKEN_LEFT_PAREN);
        case ')':
            return makeToken(TOKEN_RIGHT_PAREN);
        case '{':
            return makeToken(TOKEN_LEFT_BRACE);
        case '}':
            return makeToken(TOKEN_RIGHT_BRACE);
        case ';':
            return makeToken(TOKEN_SEMICOLON);
        case ',':
            return makeToken(TOKEN_COMMA);
        case '.':
            return makeToken(TOKEN_DOT);
        case '-':
            return makeToken(TOKEN_MINUS);
        case '+':
            return makeToken(TOKEN_PLUS);
        case '/':
            return makeToken(TOKEN_SLASH);
        case '*':
            return makeToken(TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string();
    }

    return errorToken("Unexpected character");
}
