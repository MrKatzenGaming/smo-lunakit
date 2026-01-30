#include "devgui/windows/Graphics/GraphicsHooks.h"

#include "hk/hook/InstrUtil.h"
#include "hk/hook/Trampoline.h"
#include "hk/ro/RoUtil.h"
#include "hk/sail/detail.h"
#include "hk/svc/api.h"

#include "al/Library/LiveActor/ActorInitUtil.h"
#include "al/Library/Resource/Resource.h"
#include "al/Library/Scene/Scene.h"

#include "agl/common/aglDrawContext.h"

#include "custom/agl/TextureSampler.h"
#include "custom/al/graphics/GBufferArray.h"
#include "custom/al/graphics/GraphicsPresetDirector.h"
#include "custom/sead/gfx/seadGraphicsContext.h"

#include "devgui/DevGuiHooks.h"
#include "devgui/DevGuiManager.h"

namespace al {
class GraphicsSystemInfo;
}

HkTrampoline<void, al::GraphicsSystemInfo*, const al::Resource*, const char*, const char*> CubeMapSetter =
    hk::hook::trampoline([](al::GraphicsSystemInfo* i, const al::Resource* r, const char* a, const char* b) -> void {
        CubeMapSetter.orig(i, r, a, b);
        hk::svc::OutputDebugString(a, 0x50);
    });

HkTrampoline<void, al::Scene*, const char*, int> InitGraphicsInfoHook =
    hk::hook::trampoline([](al::Scene* scene, const char* stage, int scenario) -> void {
        auto presets = DevGuiManager::instance()->getHookSettings()->getGraphicsPresetSettings();

        if (presets->mIsOverride)
            scenario = presets->mScenario;

        InitGraphicsInfoHook.orig(scene, stage, scenario);
    });

HkTrampoline<void, al::GraphicsPresetDirector*, const char*, const char*, const char*, bool> RegisterPresetHook = hk::hook::trampoline(
    [](al::GraphicsPresetDirector* presetDirector, const char* preset, const char* cubemap_location, const char* file, bool d) -> void {
        auto presets = DevGuiManager::instance()->getHookSettings()->getGraphicsPresetSettings();

        if (presets->mIsOverride) {
            preset = presets->mPreset;
            file = presets->mCubemap;
            cubemap_location = "Default";
        }

        RegisterPresetHook.orig(presetDirector, preset, cubemap_location, file, d);
    });

HkTrampoline<void, al::GraphicsPresetDirector*, const char*, int, int, int, const sead::Vector3<float>&> RequestPresetHook = hk::hook::trampoline(
    [](al::GraphicsPresetDirector* presetDirector, const char* preset, int b, int c, int d, const sead::Vector3<float>& rot) -> void {
        auto presets = DevGuiManager::instance()->getHookSettings()->getGraphicsPresetSettings();

        if (presets->mIsOverride) {
            preset = presets->mPreset;
            b = 1000;
            c = 0;
            d = 0;
        }

        RequestPresetHook.orig(presetDirector, preset, b, c, d, rot);
    });

HkTrampoline<void, void*, int, const char*, const char*> RequestCubeMapHook =
    hk::hook::trampoline([](void* cubeMapKeeper, int a, const char* location, const char* file) -> void {
        auto presets = DevGuiManager::instance()->getHookSettings()->getGraphicsPresetSettings();

        if (presets->mIsOverride) {
            a = 1000;
            location = "Default";
            file = presets->mCubemap;
        }

        RequestCubeMapHook.orig(cubeMapKeeper, a, location, file);
    });

void SkyInitHook(al::LiveActor* actor, const al::ActorInitInfo& info, const sead::SafeStringBase<char>& preset, const char* unk) {
    auto presets = DevGuiManager::instance()->getHookSettings()->getGraphicsPresetSettings();

    if (presets->mIsOverride && presets->mIsOverrideSky)
        return al::initActorWithArchiveName(actor, info, presets->mSky, unk);
    al::initActorWithArchiveName(actor, info, preset, unk);
}

void ViewportApplyHook(sead::Viewport* viewport, agl::DrawContext* ctx, agl::RenderBuffer* buffer) {
    auto gbuffer = DevGuiManager::instance()->getHookSettings()->getGBufferSettings();

    if (gbuffer->mBuffer && gbuffer->mEnable) {
        auto b = gbuffer->mBuffer;
        agl::TextureData* apply = b->mGBufBaseColor;

        switch (gbuffer->mShowType) {
        case 0:  // Base Color
            apply = b->mGBufBaseColor;
            break;
        case 1:  // Normal
            apply = b->mGBufNrmWorld;
            break;
        case 2:  // Depth
            apply = b->mGBufDepthView;
            break;
        case 3:  // Light
            apply = b->mGBufLightBuffer;
            break;
        case 4:  // Motion
            apply = b->mGBufMotionVec;
            break;
        }

        sead::GraphicsContext context;
        context.apply(ctx);

        static auto sampler = agl::TextureSampler();
        sampler.applyTextureData(*apply);

        agl::utl::ImageFilter2D::drawColorQuadTriangle(ctx, sead::Color4f::cBlack, 1.0f);
        agl::utl::ImageFilter2D::drawTextureChannel(ctx, sampler, *viewport, gbuffer->mChannel,
                                                    sead::Vector2f(1280.f / apply->mSurface._0, 720.f / apply->mSurface._2), sead::Vector2f::zero);
    }

    sead::Camera* cam;
    __asm("MOV %0, X28" : "=r"(cam));

    sead::Projection* projection;
    __asm("MOV %0, X26" : "=r"(projection));

    viewport->apply(ctx, (sead::LogicalFrameBuffer&)buffer);
}

HkTrampoline<void, al::GBufferArray*> GetTexBufferHook = hk::hook::trampoline([](al::GBufferArray* buf) -> void {
    auto gbuffer = DevGuiManager::instance()->getHookSettings()->getGBufferSettings();
    gbuffer->mBuffer = buf;

    GetTexBufferHook.orig(buf);
});

void exlSetupGraphicsHooks() {
    InitGraphicsInfoHook.installAtSym<"_ZN2al22initGraphicsSystemInfoEPNS_5SceneEPKci">();
    RegisterPresetHook.installAtSym<"_ZN2al22GraphicsPresetDirector14registerPresetEPKcS2_S2_b">();
    RequestPresetHook.installAtSym<"_ZN2al22GraphicsPresetDirector13requestPresetEPKciiiRKN4sead7Vector3IfEE">();
    RequestCubeMapHook.installAtSym<"_ZN2al19ShaderCubeMapKeeper14requestCubeMapEiPKcS2_">();

    hk::hook::writeBranchLinkAtSym<"$SkyInitHook">(SkyInitHook);

    GetTexBufferHook.installAtSym<"_ZNK2al12GBufferArray21getGBufLightBufferTexEv">();

    hk::hook::writeBranchLinkAtSym<"$ViewportApplyHook">(ViewportApplyHook);
}
