#pragma once

#include "agl/common/aglTextureData.h"

#include <prim/seadSafeString.h>

namespace agl {
class TextureSampler {
public:
    TextureSampler();
    TextureSampler(const agl::TextureData&);
    void applyTextureData(const agl::TextureData&);

    void* unk1;
    agl::TextureData mTextureData;
    char unk2[0x40];
};
};  // namespace agl
