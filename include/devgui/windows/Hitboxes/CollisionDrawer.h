#pragma once

#include "al/Library/Collision/KCollisionServer.h"
#include "al/Library/Execute/ExecutorActorExecuteBase.h"
#include "al/Library/Execute/ExecutorListActorExecute.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglGPUMemBlock.h"
#include "agl/common/aglIndexStream.h"
#include "agl/common/aglShaderLocation.h"
#include "agl/common/aglShaderProgramArchive.h"
#include "agl/common/aglVertexAttribute.h"
#include "agl/common/aglVertexBuffer.h"

class CollisionDrawer {
    SEAD_SINGLETON_DISPOSER(CollisionDrawer)

public:
    CollisionDrawer();
    ~CollisionDrawer();

    void initScene(const al::LiveActorKit* actorKit);

    void draw() const { drawImpl(false); }
    void drawDepthOnly() const { drawImpl(true); }

private:
    void drawImpl(bool depthOnly) const;

    const agl::ShaderProgram* mShader;
    const agl::ShaderProgram* mShaderDepthOnly;

    const al::CollisionDirector* mCollisionDirector = nullptr;
    agl::DrawContext* mDrawContext = nullptr;
};

class RenderedKCollisionServer : al::KCollisionServer {
public:
    struct Vertex {
        explicit Vertex(const sead::Vector3f& position, const sead::Vector3f& normal) : position(position), normal(normal) {}

        sead::Vector3f position;
        sead::Vector3f normal;
    };

    static constexpr std::array<al::UniformBlockLayout, 1> sUniformLayout = {{0, al::UniformBlock::Type::Mat3x4, 1}};

    RenderedKCollisionServer();

    void initRender();
    void draw(agl::DrawContext* context, const agl::ShaderProgram* shader, const al::CollisionParts* parts) const;

    static void installHooks();

private:
    agl::GPUMemBlock<Vertex> mVertexData;
    agl::VertexBuffer mVertexStream;
    agl::VertexAttribute mVertexAttribute;
    u32 mVertexNum;

    al::UniformBlock* mUniformBlock;
};
