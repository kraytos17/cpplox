#pragma once

#include <cstdint>

struct Value;
struct Obj;

enum class ObjType : uint8_t {
    obj_string,
};

enum class ValueType : uint8_t {
    val_bool,
    val_nil,
    val_number,
    val_obj,
};

bool isObj(const Value& value) noexcept;
Obj* asObj(const Value& value);
