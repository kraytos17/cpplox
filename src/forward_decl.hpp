#pragma once

#include <cstdint>

struct Value;
class Obj;

enum class ObjType : uint8_t {
    obj_string,
};

enum class ValueType : uint8_t {
    val_bool,
    val_nil,
    val_number,
    val_obj,
};
