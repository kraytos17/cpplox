#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include "vm.hpp"

void repl() {
    std::array<char, 1024> buffer{};
    while (true) {
        std::cout << "> ";
        if (!std::cin.getline(buffer.data(), buffer.size())) {
            std::cout << '\n';
            break;
        }

        std::string line{buffer.data()};
        [[maybe_unused]] auto x = interpret(line);
    }
}

std::optional<std::string> readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << std::format("Failed to open file: {}\n", filepath);
        return std::nullopt;
    }

    auto fileSize = file.tellg();
    if (fileSize == -1) {
        std::cerr << std::format("Could not determine file size: {}\n", filepath);
        return std::nullopt;
    }

    file.seekg(fileSize, std::ios::beg);
    std::string buffer;
    buffer.resize(static_cast<std::size_t>(fileSize));
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << std::format("Could not read file: {}\n", filepath);
        return std::nullopt;
    }

    return buffer;
}

int runFile(const std::string& filepath) {
    auto source = readFile(filepath);
    if (!source) {
        std::cerr << "Failed to read file: " << filepath << '\n';
        return 74;
    }

    InterpretResult res = interpret(*source);
    switch (res) {
        case InterpretResult::compile_error:
            return 65;
        case InterpretResult::runtime_error:
            return 70;
        case InterpretResult::ok:
            return 0;
    }
    return 0;
}


auto main(int argc, const char* argv[]) -> int {
    initVM();

    int exitCode{0};
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        exitCode = runFile(argv[1]);
    } else {
        std::cerr << "Usage: clox [path]\n";
        exitCode = 64;
    }

    freeVM();
    return exitCode;
}
