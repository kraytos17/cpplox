#include "compiler.hpp"
#include "scanner.hpp"

void compile(std::string_view source) {
    initScanner(source);
    int line = -1;
    while (true) {
        Token token = scanToken();
    }
}
