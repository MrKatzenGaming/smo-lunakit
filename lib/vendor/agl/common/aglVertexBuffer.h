#pragma once

#include <nvn/nvn.h>

#include "common/aglGPUMemAddr.h"

namespace agl {

class DrawContext;

enum class VertexStreamFormat {
    None = 0x0,
    R8 = 0x1,
    R8SN = 0x2,
    R8UI = 0x3,
    R8I = 0x4,
    R16F = 0x5,
    R16 = 0x6,
    R16SN = 0x7,
    R16UI = 0x8,
    R16I = 0x9,
    R32F = 0xA,
    R32UI = 0xB,
    R32I = 0xC,
    RG8 = 0xD,
    RG8SN = 0xE,
    RG8UI = 0xF,
    RG8I = 0x10,
    RG16F = 0x11,
    RG16 = 0x12,
    RG16SN = 0x13,
    RG16UI = 0x14,
    RG16I = 0x15,
    RG32F = 0x16,
    RG32UI = 0x17,
    RG32I = 0x18,
    RGB8 = 0x19,
    RGB8SN = 0x1A,
    RGB8UI = 0x1B,
    RGB8I = 0x1C,
    RGB16F = 0x1D,
    RGB16 = 0x1E,
    RGB16SN = 0x1F,
    RGB16UI = 0x20,
    RGB16I = 0x21,
    RGB32F = 0x22,
    RGB32UI = 0x23,
    RGB32I = 0x24,
    RGBA8 = 0x25,
    RGBA8SN = 0x26,
    RGBA8UI = 0x27,
    RGBA8I = 0x28,
    RGBA16F = 0x29,
    RGBA16 = 0x2A,
    RGBA16SN = 0x2B,
    RGBA16UI = 0x2C,
    RGBA16I = 0x2D,
    RGBA32F = 0x2E,
    RGBA32UI = 0x2F,
    RGBA32I = 0x30,
    STENCIL8 = 0x31,
    DEPTH16 = 0x32,
    DEPTH24 = 0x33,
    DEPTH32F = 0x34,
    DEPTH24_STENCIL8 = 0x35,
    DEPTH32F_STENCIL8 = 0x36,
    RGBX8_SRGB = 0x37,
    RGBA8_SRGB = 0x38,
    RGBA4 = 0x39,
    RGB5 = 0x3A,
    RGB5A1 = 0x3B,
    RGB565 = 0x3C,
    RGB10A2 = 0x3D,
    RGB10A2UI = 0x3E,
    R11G11B10F = 0x3F,
    RGB9E5F = 0x40,
    RGB_DXT1 = 0x41,
    RGBA_DXT1 = 0x42,
    RGBA_DXT3 = 0x43,
    RGBA_DXT5 = 0x44,
    RGB_DXT1_SRGB = 0x45,
    RGBA_DXT1_SRGB = 0x46,
    RGBA_DXT3_SRGB = 0x47,
    RGBA_DXT5_SRGB = 0x48,
    RGTC1_UNORM = 0x49,
    RGTC1_SNORM = 0x4A,
    RGTC2_UNORM = 0x4B,
    RGTC2_SNORM = 0x4C,
    BPTC_UNORM = 0x4D,
    BPTC_UNORM_SRGB = 0x4E,
    BPTC_SFLOAT = 0x4F,
    BPTC_UFLOAT = 0x50,
    R8_UI2F = 0x51,
    R8_I2F = 0x52,
    R16_UI2F = 0x53,
    R16_I2F = 0x54,
    R32_UI2F = 0x55,
    R32_I2F = 0x56,
    RG8_UI2F = 0x57,
    RG8_I2F = 0x58,
    RG16_UI2F = 0x59,
    RG16_I2F = 0x5A,
    RG32_UI2F = 0x5B,
    RG32_I2F = 0x5C,
    RGB8_UI2F = 0x5D,
    RGB8_I2F = 0x5E,
    RGB16_UI2F = 0x5F,
    RGB16_I2F = 0x60,
    RGB32_UI2F = 0x61,
    RGB32_I2F = 0x62,
    RGBA8_UI2F = 0x63,
    RGBA8_I2F = 0x64,
    RGBA16_UI2F = 0x65,
    RGBA16_I2F = 0x66,
    RGBA32_UI2F = 0x67,
    RGBA32_I2F = 0x68,
    RGB10A2SN = 0x69,
    RGB10A2I = 0x6A,
    RGB10A2_UI2F = 0x6B,
    RGB10A2_I2F = 0x6C,
    RGBX8 = 0x6D,
    RGBX8SN = 0x6E,
    RGBX8UI = 0x6F,
    RGBX8I = 0x70,
    RGBX16F = 0x71,
    RGBX16 = 0x72,
    RGBX16SN = 0x73,
    RGBX16UI = 0x74,
    RGBX16I = 0x75,
    RGBX32F = 0x76,
    RGBX32UI = 0x77,
    RGBX32I = 0x78,
    RGBA_ASTC_4x4 = 0x79,
    RGBA_ASTC_5x4 = 0x7A,
    RGBA_ASTC_5x5 = 0x7B,
    RGBA_ASTC_6x5 = 0x7C,
    RGBA_ASTC_6x6 = 0x7D,
    RGBA_ASTC_8x5 = 0x7E,
    RGBA_ASTC_8x6 = 0x7F,
    RGBA_ASTC_8x8 = 0x80,
    RGBA_ASTC_10x5 = 0x81,
    RGBA_ASTC_10x6 = 0x82,
    RGBA_ASTC_10x8 = 0x83,
    RGBA_ASTC_10x10 = 0x84,
    RGBA_ASTC_12x10 = 0x85,
    RGBA_ASTC_12x12 = 0x86,
    RGBA_ASTC_4x4_SRGB = 0x87,
    RGBA_ASTC_5x4_SRGB = 0x88,
    RGBA_ASTC_5x5_SRGB = 0x89,
    RGBA_ASTC_6x5_SRGB = 0x8A,
    RGBA_ASTC_6x6_SRGB = 0x8B,
    RGBA_ASTC_8x5_SRGB = 0x8C,
    RGBA_ASTC_8x6_SRGB = 0x8D,
    RGBA_ASTC_8x8_SRGB = 0x8E,
    RGBA_ASTC_10x5_SRGB = 0x8F,
    RGBA_ASTC_10x6_SRGB = 0x90,
    RGBA_ASTC_10x8_SRGB = 0x91,
    RGBA_ASTC_10x10_SRGB = 0x92,
    RGBA_ASTC_12x10_SRGB = 0x93,
    RGBA_ASTC_12x12_SRGB = 0x94,
    BGR565 = 0x95,
    BGR5 = 0x96,
    BGR5A1 = 0x97,
    A1BGR5 = 0x98,
    BGRX8 = 0x99,
    BGRA8 = 0x9A,
    BGRX8_SRGB = 0x9B,
    BGRA8_SRGB = 0x9C,
};

struct VertexStreamAttribute {
    VertexStreamFormat format;
    u32 offset;

    bool initialized;
    bool perInstance;
};

static_assert(sizeof(VertexStreamAttribute) == 0xc);

class VertexBuffer {
public:
    VertexBuffer();
    virtual ~VertexBuffer();

    void setUpBuffer(ConstGPUMemVoidAddr addr, size_t stride, size_t size);

    void flushCPUCache(u32 offset, size_t size) const;
    void* invalidateGPUCache(DrawContext* context) const;

    void setUpStream(s32 idx, VertexStreamFormat format, size_t offset, bool perInstance = false);
    void setBufferPtr(ConstGPUMemVoidAddr addr, u32 size);

private:
    void cleanUp_();

private:
    NVNbuffer mNvnBuffer;
    VertexStreamAttribute mStreamAttributes[16];
    ConstGPUMemVoidAddr mData;

    u32 mVertexStride;
    u32 mVertexSize;
    u32 mBufferSize;
};

static_assert(sizeof(VertexBuffer) == 0x120);

}  // namespace agl
