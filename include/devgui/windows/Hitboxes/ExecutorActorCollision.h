#pragma once

#include "al/Library/Collision/KCollisionServer.h"
#include "al/Library/Execute/ExecutorActorExecuteBase.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglGPUMemBlock.h"
#include "agl/common/aglVertexAttribute.h"
#include "agl/common/aglVertexBuffer.h"

namespace al {
class Scene;
}

namespace agl {
class ShaderProgramArchive;
class ShaderProgram;
}  // namespace agl

class ExecutorActorCollision : public al::ExecutorActorExecuteBase {
public:
    ExecutorActorCollision(const char* name, agl::DrawContext* context, bool depthOnly);
    void execute() const override;

private:
    const agl::ShaderProgram* mShader;
    agl::DrawContext* mContext;
};

class RenderedKCollisionServer : al::KCollisionServer {
public:
    struct Vertex {
        explicit Vertex(const sead::Vector3f& position, const sead::Vector3f& normal) : position(position), normal(normal) {}

        sead::Vector3f position;
        sead::Vector3f normal;
    };

    static constexpr std::array<al::UniformBlockLayout, 1> sUniformLayout = {{0, al::UniformBlock::Type::Vec4, 3}};

    RenderedKCollisionServer();

    void initRender();
    void draw(agl::DrawContext* context, const al::LiveActor* actor, const agl::ShaderProgram* shader) const;

    static void installHooks();

private:
    agl::GPUMemBlock<Vertex> mVertexData;
    agl::VertexBuffer mVertexStream;
    agl::VertexAttribute mVertexAttribute;
    u32 mVertexNum;

    al::UniformBlock* mUniformBlock;
};
