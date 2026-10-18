#pragma once

#include <container/seadPtrArray.h>
#include <math/seadMatrix.h>

namespace sead {
class Heap;
}

namespace al {
class GraphicsQualityInfo;
class UniformBlock;
class GraphicsSystemInfo;

class SimpleModelEnv {
public:
    SimpleModelEnv();
    ~SimpleModelEnv();

    void initialize(s32 maxViews, const GraphicsSystemInfo* graphicsSystemInfo, sead::Heap* heap);
    void updateEnv(s32 viewIdx, const sead::Matrix34f& viewMtx, const sead::Matrix44f& projMtx, const sead::Vector2f&,
                   const sead::Vector2f& screenSize, f32 near, f32 far, f32 fovy, f32, f32 invExposure, const char*);
    void setPrevViewProjMatrix(const sead::Matrix34f& viewMtx, const sead::Matrix44f& projMtx);
    void prepareModelDraw(s32 viewIdx) const;

private:
    sead::PtrArray<UniformBlock> mUBOs;

    GraphicsSystemInfo* mGraphicsSystemInfo;
    GraphicsQualityInfo* mGraphicsQualityInfo;

    sead::Matrix34f mPrevViewMtx;
    sead::Matrix44f mPrevViewProjMtx;
    s32* mBayerMtx;
};

}  // namespace al
