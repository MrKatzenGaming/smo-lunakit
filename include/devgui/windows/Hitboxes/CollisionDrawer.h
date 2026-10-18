#pragma once

#include "sead/heap/seadDisposer.h"

#include "al/Library/Collision/CollisionDirector.h"
#include "al/Library/Draw/GraphicsSystemInfo.h"
#include "al/Library/LiveActor/LiveActorKit.h"

#include "agl/common/aglRenderBuffer.h"
#include "agl/common/aglShaderProgram.h"

class CollisionDrawer {
    SEAD_SINGLETON_DISPOSER(CollisionDrawer);

public:
    CollisionDrawer();
    ~CollisionDrawer();

    void initScene(const al::LiveActorKit* actorKit);
    void draw(al::RenderVariables& renderVars, agl::RenderBuffer* renderBuffer);

private:
    const al::CollisionDirector* mCollisionDirector;

    const agl::ShaderProgram* mShader;
};
