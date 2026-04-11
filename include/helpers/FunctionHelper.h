#pragma once

#include "hk/ro/RoUtil.h"

#include <cstdint>

namespace FunctionHelper {
static ptr getAddressFromSymbol(const char* symbol) {
    return hk::ro::lookupSymbol(symbol);
}

uintptr_t findEndOfFunc(const char* symbol);
uintptr_t readLdrOffset(const char* symbol);
};  // namespace FunctionHelper
