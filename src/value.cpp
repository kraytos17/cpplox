#include "value.hpp"

void ValueArray::writeValue(Value value) { values.push_back(value); }

void ValueArray::freeValueArray() {
    values.clear();
    values.shrink_to_fit();
}
