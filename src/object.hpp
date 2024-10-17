#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <string_view>
#include "forward_decl.hpp"

class Obj {
public:
    constexpr ObjType getType() const noexcept { return m_type; }
    virtual ~Obj() = default;

protected:
    explicit Obj(ObjType t) : m_type{t} {}

private:
    ObjType m_type{};
    Obj* m_next{nullptr};
};

class ObjString : public Obj {
public:
    explicit ObjString(std::string_view str);
    ~ObjString() override = default;
    ObjString(const ObjString& other);
    ObjString(ObjString&& other) noexcept = default;
    ObjString& operator=(const ObjString& other);
    ObjString& operator=(ObjString&& other) noexcept = default;

    constexpr size_t getLength() const noexcept { return m_length; }

    constexpr auto getChars() const noexcept {
        return isSmallString() ? std::string_view(m_ssoString.data(), m_length)
                               : std::string_view(m_chars.get(), m_length);
    }

    constexpr const char* getCString() const noexcept { return isSmallString() ? m_ssoString.data() : m_chars.get(); }

private:
    static constexpr auto SSO_THRESHOLD = 23;

    std::size_t m_length{0};
    std::unique_ptr<char[]> m_chars{nullptr};
    std::array<char, SSO_THRESHOLD + 1> m_ssoString;

    constexpr bool isSmallString() const noexcept { return m_length <= SSO_THRESHOLD; }
};

std::unique_ptr<ObjString> copyString(const char* chars, int length);
void printObj(const Value& value);
