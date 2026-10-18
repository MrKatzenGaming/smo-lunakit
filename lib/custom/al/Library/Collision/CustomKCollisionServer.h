#pragma once

#include "al/Library/Collision/KCollisionServer.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglVertexAttribute.h"
#include "agl/common/aglVertexBuffer.h"

namespace al {

class CustomKCollisionServer : KCollisionServer {
public:
    struct Vertex {
        explicit Vertex(const sead::Vector3f& position, const sead::Vector3f& normal, f32 edgeDistance)
            : position(position), normal(normal), edgeDistance(edgeDistance) {}

        sead::Vector3f position;
        sead::Vector3f normal;
        f32 edgeDistance;
    };

    static constexpr std::array<UniformBlockLayout, 1> sUniformLayout = {{0, UniformBlock::Type::Mat3x4, 1}};

    CustomKCollisionServer();

    void initRender();
    void draw(agl::DrawContext* context, const agl::ShaderProgram* shader, const CollisionParts* parts) const;

    static void installHooks();

private:
    agl::GPUMemBlock<Vertex> mVertexData;
    agl::VertexBuffer mVertexStream;
    agl::VertexAttribute mVertexAttribute;
    u32 mVertexNum;

    UniformBlock* mUniformBlock;
};

}  // namespace al
