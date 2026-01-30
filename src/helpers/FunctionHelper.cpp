#include "helpers/FunctionHelper.h"

static constexpr const u32 RetValue = 0xD65F03C0;  // bytecode of "ret"

uintptr_t FunctionHelper::findEndOfFunc(const char* symbol) {
    u32* ptr = (u32*)getAddressFromSymbol(symbol);
    // naive implementation, ignores the fact that functions can have multiple returns
    while (*ptr != RetValue)
        ptr++;
    return (uintptr_t)ptr;
}
