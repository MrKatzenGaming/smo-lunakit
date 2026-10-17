#include "devgui/windows/Hitboxes/CollisionDrawer.h"

#include "hk/hook/Trampoline.h"

#include "sead/gfx/seadGraphicsContextMTR.h"
#include "sead/heap/seadDisposer.h"
#include "sead/prim/seadSafeString.h"

#include "al/Library/Collision/CollisionDirector.h"
#include "al/Library/Collision/CollisionPartsKeeperOctree.h"
#include "al/Library/Collision/CollisionPartsKeeperUtil.h"
#include "al/Library/Draw/GraphicsSystemInfo.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Model/SimpleModelEnv.h"
#include "al/Library/Play/Draw/RenderVariables.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglDrawContext.h"
#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglVertexBuffer.h"

#include "custom/al/Library/Collision/CustomKCollisionServer.h"

#include "helpers/ShaderHelper.h"
#include "logger/Logger.hpp"

SEAD_SINGLETON_DISPOSER_IMPL(CollisionDrawer);

HkTrampoline hookEndInit = [](TrampolineStatic(), al::LiveActorKit* self) -> void {
    SEAD_INSTANCE(CollisionDrawer)->initScene(self);
    orig(self);
};

static void drawCollision(al::RenderVariables& renderVars, agl::RenderBuffer* renderBuffer) {
    SEAD_INSTANCE(CollisionDrawer)->draw(renderVars, renderBuffer);
}

static void drawCollisionWrapper() {
    asm volatile("stp x29, x30, [sp, #-32]\n"
                 "add x29, sp, #-32\n"
                 "stp x0, x1, [sp, #16]\n"

                 // Load al::RenderVariables* from stack
                 "add x0, sp, #0x2, LSL #12\n"
                 "add x0, x0, #0x328\n"
                 // Load agl::RenderBuffer* from register
                 "mov x1, x28\n"

                 // Only offset stack now, such that the previous stack calculation matches vanilla
                 "add sp, sp, #-32\n"

                 "bl %0\n"

                 "ldp x0, x1, [sp, #16]\n"
                 "ldp x29, x30, [sp], #32\n"
                 :
                 : "X"(drawCollision));

    // Overwitten instruction
    asm volatile("ldur x1, [x29, #-0xa8]");
}

CollisionDrawer::CollisionDrawer() {
    mShader = SEAD_INSTANCE(ShaderHelper)->getShaderProgram("lkRenderCollision");

    hookEndInit.installAtSym<"_ZN2al12LiveActorKit7endInitEv">();
    hk::hook::writeBranchLinkAtSym<"$drawCollisionHook">(drawCollisionWrapper);
}

CollisionDrawer::~CollisionDrawer() = default;

void CollisionDrawer::initScene(const al::LiveActorKit* actorKit) {
    mCollisionDirector = actorKit->getCollisionDirector();
}

static void drawNode(agl::DrawContext* drawCtx, const agl::ShaderProgram* shader, const al::CollisionPartsKeeperOctree::OctNode* node) {
    for (const auto parts : node->collisionParts) {
        if (parts->mSpecialPurpose != nullptr)
            continue;

        const auto server = reinterpret_cast<al::CustomKCollisionServer*>(parts->getKCollisionServer());
        server->draw(drawCtx, shader, parts);
    }

    for (s32 i = 0; i < 8; i++)
        if (node->octants[i])
            drawNode(drawCtx, shader, node->octants[i]);
}

void CollisionDrawer::draw(al::RenderVariables& renderVars, agl::RenderBuffer* renderBuffer) {
    if (!mCollisionDirector) {
        Logger::log("skip");
        return;
    }

    auto drawContext = renderVars.mRenderInfo->drawContext;
    renderVars.mSimpleModelEnv->prepareModelDraw(renderVars.mRenderInfo->getViewIndex());

    // Render onto the LDR buffer, but with the HDR depth buffer
    auto origDepth = renderBuffer->mRenderTargetDepth;
    renderBuffer->mRenderTargetDepth = &renderVars.mDepthTarget;
    renderBuffer->bind(drawContext);
    renderVars.mViewport.apply(drawContext, *renderBuffer);

    sead::GraphicsContextMRT gfxContext;
    gfxContext.apply(drawContext);

    drawNode(drawContext, mShader, &mCollisionDirector->mRootOctree->mRoot);

    renderBuffer->mRenderTargetDepth = origDepth;
}
