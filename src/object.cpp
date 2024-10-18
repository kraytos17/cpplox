#include "object.hpp"
#include <cstring>
#include <print>
#include "forward_decl.hpp"
#include "inline_decl.hpp"
#include "value.hpp"

ObjString::ObjString(std::string_view str) : Obj(ObjType::obj_string), m_length(str.size()) {
    if (isSmallString()) {
        std::memcpy(m_ssoString.data(), str.data(), m_length);
        m_ssoString[m_length] = '\0';
    } else {
        m_chars = std::make_unique<char[]>(m_length + 1);
        std::memcpy(m_chars.get(), str.data(), m_length);
        m_chars[m_length] = '\0';
    }
}

ObjString::ObjString(const ObjString& other) : Obj(ObjType::obj_string), m_length(other.m_length) {
    if (isSmallString()) {
        std::memcpy(m_ssoString.data(), other.m_ssoString.data(), m_length + 1);
    } else {
        m_chars = std::make_unique<char[]>(m_length + 1);
        std::memcpy(m_chars.get(), other.m_chars.get(), m_length + 1);
    }
}

ObjString& ObjString::operator=(const ObjString& other) {
    if (this != &other) {
        m_length = other.m_length;
        if (isSmallString()) {
            std::memcpy(m_ssoString.data(), other.m_ssoString.data(), m_length + 1);
        } else {
            m_chars = std::make_unique<char[]>(m_length + 1);
            std::memcpy(m_chars.get(), other.m_chars.get(), m_length + 1);
        }
    }

    return *this;
}

std::unique_ptr<ObjString> copyString(const char* chars, int length) {
    auto heapChars = std::make_unique<char[]>(length + 1);
    std::memcpy(heapChars.get(), chars, length);
    heapChars[length] = '\0';

    return std::make_unique<ObjString>(std::string_view(heapChars.get(), length));
}

void printObj(const Value& value) {
    switch (asObj(value)->getType()) {
        case ObjType::obj_string:
            std::print("{}", asCString(value));
            break;
    }
}
