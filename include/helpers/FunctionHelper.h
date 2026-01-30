#pragma once

#include "nn/ro.h"

#include <cstdint>

namespace FunctionHelper {
static uintptr_t getAddressFromSymbol(const char* symbol) {
    uintptr_t result;
    nn::ro::LookupSymbol(&result, symbol);
    return result;
}

uintptr_t findEndOfFunc(const char* symbol);
};  // namespace FunctionHelper
