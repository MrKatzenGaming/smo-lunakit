#pragma once

#include <common/aglUniformBlock.h>
#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace agl {
class ShaderProgram;
class UniformBlock;
}  // namespace agl

namespace al {

class GpuMemAllocator;

class UniformBlock : public agl::UniformBlock {
public:
    UniformBlock();
    ~UniformBlock() override;

    void swap();

private:
    s32 mActiveSwapIndex = 0;
};

static_assert(sizeof(UniformBlock) == 0x80);

class UniformBlockSetter {
public:
    UniformBlockSetter(UniformBlock* block, s32 instanceIdx);
    ~UniformBlockSetter();

private:
    UniformBlock* mUniformBlock;
    s32 mInstanceIndex;
};

static_assert(sizeof(UniformBlockSetter) == 0x10);

struct UniformBlockLayout {
    s32 index;
    UniformBlock::Type type;
    s32 elementNum;
};

static_assert(sizeof(UniformBlockLayout) == 0xc);

UniformBlock* createUniformBlock(const UniformBlockLayout* layouts, s32 layoutNum, sead::Heap* heap, s32 swapNum);
void declareUniformBlock(agl::UniformBlock* block, const UniformBlockLayout* layouts, s32 layoutNum, sead::Heap* heap);

UniformBlock* createUniformBlockInstanceNum(const UniformBlockLayout* layouts, s32 layoutNum, s32 instanceNum, sead::Heap*, s32 swapNum);
UniformBlock* allocUniformBlock(const UniformBlockLayout* layouts, s32 layoutNum, GpuMemAllocator* allocator, s32 swapNum);

bool setUniformBlockToShader(UniformBlock* block, agl::DrawContext* context, const agl::ShaderProgram& shader, const char* name, s32 instanceIdx);

}  // namespace al
