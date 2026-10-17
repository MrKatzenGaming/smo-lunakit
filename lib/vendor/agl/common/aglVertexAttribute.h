#pragma once

#include <nvn/nvn.h>
#include <nvn/nvn_FuncPtrInline.h>

#include <container/seadBuffer.h>
#include <prim/seadBitUtil.h>

#include "common/aglIndexStream.h"
#include "common/aglVertexEnum.h"

namespace sead {
class Heap;
}  // namespace sead

namespace agl {

class DrawContext;
class VertexBuffer;

class VertexAttribute {
public:
    VertexAttribute();
    virtual ~VertexAttribute();

    void cleanUp();
    void destroy();

    void create(u32 streamNum, sead::Heap* heap);

    void setVertexStream(s32 attributeIdx, const VertexBuffer* stream, u32 streamAttributeIdx);
    const VertexBuffer* getVertexStream(s32 attributeIdx, u32* outStreamAttributeIdx) const;

    void setUp();
    void activate(DrawContext* context) const;
    void disableAttributeAll(DrawContext* context);

    void drawArrays(DrawContext* context, VertexStreamTopology topology, u32 first, u32 num) const {
        activate(context);
        nvnCommandBufferDrawArrays(context->getCommandBuffer()->ToData()->pNvnCommandBuffer, sead::BitUtil::bitCast<NVNdrawPrimitive>(topology),
                                   first, num);
    }

    void drawElements(DrawContext* context, const IndexStream& stream) const {
        activate(context);
        stream.drawElements(context);
    }

private:
    struct Attribute_ {
        VertexBuffer* buffer;
        s8 streamAttribute;
        s8 streamIndex;
    };

    static_assert(sizeof(Attribute_) == 0x10);

    void disableVertexBuffer_(Attribute_* attribute);
    void enableVertexBuffer_(Attribute_* attribute, const VertexBuffer* buffer, u32 streamAttributeIdx);

    Attribute_ mAttributes[16];
    sead::Buffer<VertexBuffer*> mStreams;

    struct {
        bool mIsSetUp : 1;
        bool mIsAllocated : 1;
    };

    NVNvertexAttribState mAttribState[16];
    NVNvertexStreamState mStreamState[16];
};

static_assert(sizeof(VertexAttribute) == 0x1e0);

}  // namespace agl
