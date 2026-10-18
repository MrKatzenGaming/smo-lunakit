#pragma once

#include <type_traits>

#include <nvn/nvn.h>
#include <nvn/nvn_FuncPtrInline.h>

#include "common/aglDrawContext.h"
#include "common/aglGPUMemAddr.h"
#include "prim/seadBitUtil.h"

namespace agl {

class DrawContext;

enum class IndexStreamFormat {
    None = 0x0,
    U16 = 0x1,
    U32 = 0x2,
};
enum class IndexStreamTopology {
    Points = 0x0,
    Lines = 0x1,
    LineLoop = 0x2,
    LineStrip = 0x3,
    Triangles = 0x4,
    TriangleStrip = 0x5,
    TriangleFan = 0x6,
    Quads = 0x7,
    QuadStrip = 0x8,
    Polygon = 0x9,
    LinesAdjacency = 0xA,
    LineStripAdjacency = 0xB,
    TrianglesAdjacency = 0xC,
    TriangleStripAdjacency = 0xD,
    Patches = 0xE,
};

class IndexStream {
public:
    IndexStream();
    virtual ~IndexStream();

    void flushCPUCache(u32 offset, u32 count) const;
    void* invalidateGPUCache(DrawContext* context) const;

    template <typename T>
    void setUpStream(ConstGPUMemVoidAddr data, u32 count) {
        if constexpr (std::is_same<T, u16>())
            setUpStream_(data, IndexStreamFormat::U16, count);
        else if (std::is_same<T, u32>())
            setUpStream_(data, IndexStreamFormat::U32, count);
        else
            static_assert(!std::is_same<T, T>(), "Unsupported index type");
    }
    void setBufferPtr(ConstGPUMemVoidAddr data, u32 count);

    void setTopology(IndexStreamTopology topology) { mTopology = topology; }

    void drawElements(DrawContext* context) const {
        nvnCommandBufferDrawElements(context->getCommandBuffer()->ToData()->pNvnCommandBuffer,
                                     sead::BitUtil::bitCast<NVNdrawPrimitive>(mTopology),
                                     sead::BitUtil::bitCast<NVNindexType>(mFormat), mIndexCount,
                                     nvnBufferGetAddress(&mNvnBuffer));
    }

private:
    void cleanUp_();
    void setUpStream_(ConstGPUMemVoidAddr data, IndexStreamFormat format, u32 count);

private:
    IndexStreamFormat mFormat;
    IndexStreamTopology mTopology;

    ConstGPUMemVoidAddr mData;

    u32 mIndexCount;
    u32 mIndexSize;

    NVNbuffer mNvnBuffer;
};

static_assert(sizeof(IndexStream) == 0x60);

}  // namespace agl
