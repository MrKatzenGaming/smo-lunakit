#pragma once

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

class ExecutorListActorCollision : public al::ExecutorListActorExecuteBase {
public:
    ExecutorListActorCollision(const char* name, s32 size);
    al::ExecutorActorExecuteBase* createExecutor(const char* name) const override;

    static void init();
};

class ExecutorActorCollision : public al::ExecutorActorExecuteBase {
public:
    ExecutorActorCollision(const char* name);
    void execute() const override;
};

class CollisionDrawer {
public:
    struct Vertex {
        static constexpr std::array<al::UniformBlockLayout, 1> sLayout = {{0, al::UniformBlock::Type::Vec4, 4}};

        Vertex(sead::Vector3f position) : position(position) {}

        sead::Vector3f position;
    };

    CollisionDrawer();

    void draw(agl::DrawContext* context, const al::Scene* scene) const;

    void genCube(const sead::Vector3f& pos, f32 size);

private:
    static constexpr size_t sMaxVertices = 1024;
    static constexpr size_t sMaxIndices = 1024 * 3;

    agl::ShaderProgramArchive* mArchive;
    agl::ShaderProgram* mProgram;

    agl::GPUMemBlock<Vertex> mVertexData;
    agl::VertexBuffer mVertexStream;
    agl::VertexAttribute mVertexAttribute;

    agl::GPUMemBlock<u16> mIndexData;
    agl::IndexStream mIndexStream;

    al::UniformBlock* mUniformBlock;

    u32 mVertexCount = 0;
    u32 mIndexCount = 0;
};
