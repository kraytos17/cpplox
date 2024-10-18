#include <array>
#include <cstddef>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <span>
#include <string_view>
#include "vm.hpp"

void repl() {
    std::array<char, 1024> buffer{};
    while (true) {
        std::print("> ");
        if (!std::cin.getline(buffer.data(), buffer.size())) {
            std::println();
            break;
        }

        std::string_view line{buffer.data()};
        [[maybe_unused]] auto x = interpret(line);
    }
}

std::optional<std::string> readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file) {
        std::println(stderr, "Failed to open file: {}", filepath);
        return std::nullopt;
    }

    auto fileSize = file.tellg();
    if (fileSize == -1) {
        std::println(stderr, "Could not determine file size: {}", filepath);
        return std::nullopt;
    }

    file.seekg(0, std::ios::beg);
    std::string buffer(static_cast<std::size_t>(fileSize), '\0');
    if (!file.read(buffer.data(), fileSize)) {
        std::println(stderr, "Could not read file: {}", filepath);
        return std::nullopt;
    }

    return buffer;
}

int runFile(const std::string& filepath) {
    auto source = readFile(filepath);
    if (!source) {
        std::println(stderr, "Failed to read file: {}", filepath);
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
    std::span args(argv, static_cast<std::size_t>(argc));

    if (args.size() == 1) {
        repl();
    } else if (args.size() == 2) {
        exitCode = runFile(args[1]);
    } else {
        std::println(stderr, "Usage: clox [path]");
        exitCode = 64;
    }

    freeVM();
    return exitCode;
}
