#pragma once

#include <basis/seadTypes.h>

namespace agl {

class TextureSampler {
private:
    u8 _0[0x170];
};

static_assert(sizeof(TextureSampler) == 0x170);

}  // namespace agl
