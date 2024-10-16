#include "object.hpp"
#include "inline_decl.hpp"
#include "value.hpp"

constexpr ObjType getObjType(const Obj& obj) noexcept { return obj.type; }

constexpr bool isObjType(const Value& value, ObjType type) noexcept {
    return isObj(value) && (asObj(value)->type == type);
}

constexpr bool isObjString(const Value& value) noexcept { return isObjType(value, ObjType::obj_string); }

ObjString::ObjString(std::string_view str) : obj{ObjType::obj_string}, length{str.size()} {
    if (length <= SSO_THRESHOLD) {
        std::memcpy(sso_string.data(), str.data(), length);
        sso_string[length] = '\0';
    } else {
        chars = new char[length + 1];
        std::memcpy(chars, str.data(), length);
        chars[length] = '\0';
    }
}

ObjString::~ObjString() {
    if ((length > SSO_THRESHOLD) && chars) {
        delete[] chars;
    }
}

ObjString::ObjString(const ObjString& other) : obj{other.obj}, length{other.length} {
    if (length <= SSO_THRESHOLD) {
        std::memcpy(sso_string.data(), other.sso_string.data(), length + 1);
    } else {
        chars = new char[length + 1];
        std::memcpy(chars, other.chars, length + 1);
    }
}

ObjString::ObjString(ObjString&& other) noexcept : obj{other.obj}, length(other.length), chars{other.chars} {
    if (length <= SSO_THRESHOLD) {
        std::memcpy(sso_string.data(), other.sso_string.data(), length + 1);
    }
    other.chars = nullptr;
}

ObjString& ObjString::operator=(const ObjString& other) {
    if (this != &other) {
        if (length > SSO_THRESHOLD) {
            delete[] chars;
        }
        length = other.length;
        if (length <= SSO_THRESHOLD) {
            std::memcpy(sso_string.data(), other.sso_string.data(), length + 1);
        } else {
            chars = new char[length + 1];
            std::memcpy(chars, other.chars, length + 1);
        }
    }
    return *this;
}

ObjString& ObjString::operator=(ObjString&& other) noexcept {
    if (this != &other) {
        if (length > SSO_THRESHOLD) {
            delete[] chars;
        }
        length = other.length;
        if (length <= SSO_THRESHOLD) {
            std::memcpy(sso_string.data(), other.sso_string.data(), length + 1);
        } else {
            chars = other.chars;
            other.chars = nullptr;
        }
    }
    return *this;
}

std::string_view ObjString::getChars() const noexcept {
    if (length <= SSO_THRESHOLD) {
        return std::string_view(sso_string.data(), length);
    } else {
        return std::string_view(chars, length);
    }
}
