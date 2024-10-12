#include <array>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <ostream>
#include "vm.hpp"

void repl() {
    std::array<char, 1024> buffer;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::cin.getline(buffer.data(), buffer.size())) {
            std::cout << '\n';
            break;
        }

        std::string line{buffer.data()};
        interpret(line);
    }
}

std::optional<std::string> readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file: " << filepath << '\n';
        return std::nullopt;
    }

    auto fileSize = file.tellg();
    if (fileSize == -1) {
        std::cerr << "Could not determine file size: " << filepath << '\n';
        return std::nullopt;
    }

    file.seekg(fileSize, std::ios::beg);
    std::string buffer;
    buffer.resize(static_cast<std::size_t>(fileSize));
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << "Could not read file: " << filepath << std::endl;
        return std::nullopt;
    }

    return buffer;
}

void runFile(const std::string& filepath) {
    auto source = readFile(filepath);
    if (!source)
        std::exit(74);

    InterpretResult res = interpret(*source);
    switch (res) {
        case INTERPRET_COMPILE_ERROR:
            std::exit(65);
        case INTERPRET_RUNTIME_ERROR:
            std::exit(70);
        case INTERPRET_OK:
            return;
    }
}

int main(int argc, const char* argv[]) {
    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        std::cerr << "Usage: clox [path]\n";
        return 64;
    }

    freeVM();
    return 0;
}
