#pragma once

#include <cassert>
#include <string_view>
#include "object.hpp"
#include "value.hpp"

inline constexpr bool isObj(const Value& value) noexcept { return value.type == ValueType::val_obj; }

inline constexpr Obj* asObj(const Value& value) {
    assert(isObj(value) && "Value is not an object.");
    return std::get<Obj*>(value.as);
}

inline constexpr bool isBool(const Value& value) noexcept { return value.type == ValueType::val_bool; }

inline constexpr bool isNil(const Value& value) noexcept { return value.type == ValueType::val_nil; }

inline constexpr bool isNumber(const Value& value) noexcept { return value.type == ValueType::val_number; }

inline constexpr Value boolValue(bool value) { return Value(value); }

inline constexpr Value nilValue() { return Value{}; }

inline constexpr Value numberValue(double value) { return Value(value); }

inline constexpr Value objValue(Obj* obj) { return Value(obj); }

inline constexpr bool valuesEq(Value a, Value b) {
    if (a.type != b.type) {
        return false;
    }

    switch (a.type) {
        case ValueType::val_bool:
            return std::get<bool>(a.as) == std::get<bool>(b.as);
        case ValueType::val_nil:
            return true;
        case ValueType::val_number:
            return std::get<double>(a.as) == std::get<double>(b.as);
        case ValueType::val_obj:
            return std::get<Obj*>(a.as) == std::get<Obj*>(b.as);
        default:
            return false;
    }
}

inline constexpr bool asBool(const Value& value) {
    assert(isBool(value) && "Value is not a boolean.");
    return std::get<bool>(value.as);
}

inline constexpr double asNumber(const Value& value) {
    assert(isNumber(value) && "Value is not a number.");
    return std::get<double>(value.as);
}

inline constexpr ObjString* asObjString(const Value& value) { return static_cast<ObjString*>(asObj(value)); }

inline constexpr const char* asCString(const Value& value) { return asObjString(value)->getCString(); }

inline constexpr std::string_view asStringView(const Value& value) { return asObjString(value)->getStringView(); }
