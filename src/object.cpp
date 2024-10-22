#include "object.hpp"
#include <cstring>
#include <memory>
#include <print>
#include "forward_decl.hpp"
#include "inline_decl.hpp"
#include "value.hpp"

static void copyStringData(char* dest, const char* src, int length) {
    std::memcpy(dest, src, length);
    dest[length] = '\0';
}

ObjString::ObjString(std::string_view str) : Obj(ObjType::obj_string), m_length(str.size()) {
    if (isSmallString()) {
        copyStringData(m_ssoString.data(), str.data(), m_length);
    } else {
        m_chars = std::make_unique_for_overwrite<char[]>(m_length + 1);
        copyStringData(m_chars.get(), str.data(), m_length);
        m_chars_cap = m_length;
    }
}

ObjString::ObjString(const ObjString& other) : Obj(ObjType::obj_string), m_length(other.m_length) {
    if (isSmallString()) {
        copyStringData(m_ssoString.data(), other.m_ssoString.data(), m_length);
    } else {
        m_chars = std::make_unique_for_overwrite<char[]>(m_length + 1);
        copyStringData(m_chars.get(), other.m_chars.get(), m_length);
        m_chars_cap = m_length;
    }
}

ObjString& ObjString::operator=(const ObjString& other) {
    if (this != &other) {
        m_length = other.m_length;
        if (isSmallString()) {
            copyStringData(m_ssoString.data(), other.m_ssoString.data(), m_length);
        } else {
            if (!m_chars || m_length > m_chars_cap) {
                m_chars = std::make_unique_for_overwrite<char[]>(m_length + 1);
                m_chars_cap = m_length;
            }
            copyStringData(m_chars.get(), other.m_chars.get(), m_length);
        }
    }

    return *this;
}

std::unique_ptr<ObjString> copyString(const char* chars, int length) {
    return std::make_unique<ObjString>(std::string_view(chars, length));
}

void printObj(const Value& value) {
    if (asObj(value)->getType() == ObjType::obj_string) {
        std::print("{}", asCString(value));
    }
}
