#pragma once

#include <nvn/nvn.h>

#include "gfx/seadColor.h"
#include "prim/seadBitFlag.h"

namespace sead {
class DrawContext;

class GraphicsContextMRT {
public:
    struct BlendTarget {
        NVNblendFunc srcFunc;
        NVNblendFunc srcFuncAlpha;
        NVNblendFunc dstFunc;
        NVNblendFunc dstFuncAlpha;

        NVNblendEquation modeRGB;
        NVNblendEquation modeAlpha;
    };

    GraphicsContextMRT();
    virtual ~GraphicsContextMRT() {}

    void apply(DrawContext* context) const;
    void applyAlphaTest(DrawContext* context) const;
    void applyDepthAndStencilTest(DrawContext* context) const;
    void applyColorMask(DrawContext* context) const;
    void applyBlendAndFastZ(DrawContext* context) const;
    void applyBlendConstantColor(DrawContext* context) const;
    void applyCullingAndPolygonModeAndPolygonOffset(DrawContext* context) const;

private:
    bool mEnableDepthTest;
    bool mEnableDepthWrite;

    NVNdepthFunc mDepthFunc;
    NVNface mPolygonCullFace;

    BitFlag32 mEnableBlendFlags;
    BlendTarget mBlendTargets[8];
    Color4f mBlendConstant;

    bool mEnableAlphaTest;
    NVNalphaFunc mAlphaFunc;
    f32 mAlphaRef;

    BitFlag32 mEnableColorFlags;

    bool mEnableStebcilTest;
    NVNstencilFunc mStencilFunc;
    s32 mStencilRef;
    u32 mStencilReadMask;
    NVNstencilOp mStencilFailOp;
    NVNstencilOp mStencilFailDepthOp;
    NVNstencilOp mStencilPassDepthOp;

    s32 _114;
    NVNpolygonMode mPolygonMode;
    s32 _11c;
    BitFlag8 mEnablePolygonOffsetFlags;
    u8 _121;
};

static_assert(sizeof(GraphicsContextMRT) == 0x128);

}  // namespace sead
