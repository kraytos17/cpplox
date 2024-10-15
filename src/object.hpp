#pragma once

#include <array>
#include <cstddef>
#include <cstring>
#include <string_view>
#include "forward_decl.hpp"

struct Obj {
    ObjType type{};
};

ObjType getObjType(const Obj& obj) noexcept;
bool isObjType(const Value& value, ObjType type) noexcept;
bool isObjString(const Value& value) noexcept;

struct ObjString {
    static constexpr auto SSO_THRESHOLD = 23;

    char* chars{nullptr};
    Obj obj{};
    size_t length{0};
    std::array<char, SSO_THRESHOLD + 1> sso_string{};

    ObjString(std::string_view str);
    ~ObjString();
    ObjString(const ObjString& other);
    ObjString(ObjString&& other) noexcept;
    ObjString& operator=(const ObjString& other);
    ObjString& operator=(ObjString&& other) noexcept;

    std::string_view getChars() const noexcept;
};
