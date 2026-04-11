#include "helpers/FunctionHelper.h"

#include "hk/sail/detail.h"

static constexpr const u32 RetValue = 0xD65F03C0;  // bytecode of "ret"

uintptr_t FunctionHelper::findEndOfFunc(const char* symbol) {
    u32* ptr = (u32*)getAddressFromSymbol(symbol);
    // naive implementation, ignores the fact that functions can have multiple returns
    while (*ptr != RetValue)
        ptr++;
    return (uintptr_t)ptr;
}

uintptr_t FunctionHelper::readLdrOffset(const char* symbol) {
#ifdef __aarch64__
    const ptr* insn = reinterpret_cast<const ptr*>(hk::sail::lookupSymbolFromDb(symbol));
    if (insn == nullptr)
        return 0;

    // Read only the first instruction: `ldr <Rt>, [x0, #imm]` (unsigned offset).
    u32 op = insn[0];

    bool isLdrUnsignedOffset = ((op >> 22) & 0xFF) == 0b11100101;
    if (!isLdrUnsignedOffset)
        return 0;

    u32 baseReg = (op >> 5) & 0x1F;
    if (baseReg != 0)
        return 0;

    u32 imm12 = (op >> 10) & 0xFFF;
    u32 size = (op >> 30) & 0x3;

    return uintptr_t(imm12) << size;
#endif

    return 0;
}
