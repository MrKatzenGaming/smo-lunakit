#pragma once

#include <gfx/seadViewport.h>
#include <math/seadMatrix.h>

#include "common/aglRenderBuffer.h"
#include "common/aglRenderTarget.h"
#include "common/aglTextureSampler.h"

namespace agl {
class DrawContext;
}  // namespace agl

namespace al {

class HdrEncode;
class SimpleModelEnv;
class SceneCameraInfo;
class GraphicsSystemInfo;
class GraphicsQualityInfo;
class EffectSystem;
class ExecuteDirector;
class GBufferArray;
class LightPrePass;
class Projection;

struct ViewInfo {
    s32 _0;
    s32 _4;

    const sead::Matrix34f& viewMtx;
    sead::Matrix34f viewInvMtx;

    const sead::Matrix44f& projMtx;
    sead::Matrix44f projInvMtx;

    u8 _88[0x10];
};

static_assert(sizeof(ViewInfo) == 0x98);

struct GraphicsRenderInfo {
    s32 getViewIndex() const;
    HdrEncode* getHdrEncode() const;

    agl::DrawContext* drawContext;
    s32* viewIndex;
    SceneCameraInfo* cameraInfo;
    SimpleModelEnv* modelEnv;
    GraphicsSystemInfo* systemInfo;
    GraphicsQualityInfo* qualityInfo;
};

static_assert(sizeof(GraphicsRenderInfo) == 0x30);

class RenderVariables {
public:
    RenderVariables(agl::DrawContext* drawContext, GraphicsRenderInfo* renderInfo, ExecuteDirector* executeDirector, EffectSystem* effectSystem,
                    const SimpleModelEnv* modelEnv, const ViewInfo& viewInfo, s32 width, s32 height, bool, bool, bool depthBuffer);
    ~RenderVariables();

    GraphicsSystemInfo* getGraphicsSystemInfo() const;

    void allocDepthBuffer(agl::DrawContext*);
    void bindRenderBuffer(agl::DrawContext*) const;
    void drawPeripheryZOnly(agl::DrawContext*) const;
    void drawReconstructPeriphery(agl::DrawContext*);

    void* getRenderTargetColorTex() const;

    s32 getWidth() const;
    s32 getHeight() const;

public:
    GraphicsRenderInfo* mRenderInfo;
    s32 mWidth;
    s32 mHeight;

    ExecuteDirector* mExecuteDirector;
    EffectSystem* mEffectSystem;
    void* _20;
    SimpleModelEnv* mSimpleModelEnv;
    ViewInfo mViewInfo;
    GBufferArray* mGBufferArray;
    LightPrePass* mLightPrePass;

    agl::RenderBuffer mRenderBuffer;
    sead::Viewport mViewport;

    agl::RenderTargetColor mDeferredShadingTarget;
    agl::TextureData* mDeferredShadingBuffer;

    agl::RenderTargetDepth mDepthTarget;
    agl::TextureData* mDepthBuffer;
    agl::TextureSampler mDepthSampler;

    agl::RenderTargetDepth mHalfDepthTarget;
    agl::TextureData* mHalfDepthBuffer;
    agl::TextureSampler mHalfDepthSampler;

    agl::RenderTargetDepth mQuarterDepthTarget;
    agl::TextureData* mQuarterDepthBuffer;
    agl::TextureSampler mQuarterDepthSampler;

    agl::TextureData* mHalfLinearDepthBuffer;
    agl::TextureData* mQuarterLinearDepthBuffer;
    agl::TextureData* mPeripheryBuffer;

    bool _bc8;

    const sead::Camera& mCamera;
    const al::Projection& mProjection;
    f32 mNear;
    f32 mFar;

    bool _be8;
    bool mEnableDepthBuffer;
    bool _bea;
};

static_assert(sizeof(RenderVariables) == 0xbf0);

}  // namespace al
