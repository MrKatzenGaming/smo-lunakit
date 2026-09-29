#include "devgui/windows/Hitboxes/CollisionDrawer.h"

#include "sead/gfx/seadCamera.h"
#include "sead/gfx/seadGraphics.h"
#include "sead/gfx/seadProjection.h"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"

#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/Execute/ExecutorListActorDraw.h"
#include "al/Library/Resource/Resource.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglResBinaryShaderArchive.h"
#include "agl/common/aglResShaderArchive.h"
#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglShaderProgramArchive.h"
#include "agl/common/aglVertexBuffer.h"

#include "helpers/ExecuteHelper.h"
#include "logger/Logger.hpp"

static constexpr al::ExecuteOrder collisionOrder = {"Collision[LunaKit]", "ActorDrawCollision[LunaKit]", 32, "地形"};

CollisionDrawer::CollisionDrawer() {
    // Shader Program
    const auto* shaderRes = new al::Resource("ShaderData/lkRenderCollision");

    sead::Graphics::instance()->lockDrawContext();
    mArchive = new agl::ShaderProgramArchive();
    mArchive->createWithOption(agl::ResBinaryShaderArchive(shaderRes->getOtherFile("lkRenderCollision.sharcb")), agl::ResShaderArchive(), 0, nullptr);
    mArchive->setUp();
    sead::Graphics::instance()->unlockDrawContext();

    mProgram = mArchive->mShaderPrograms.get(mArchive->searchShaderProgramIndex("lkRenderCollision"));

    // Buffer Data
    mVertexData.alloc(sMaxVertices, nullptr);
    mIndexData.alloc(sMaxIndices, nullptr);

    // Vertex Attribute
    mVertexStream.setUpBuffer(mVertexData, sizeof(Vertex), sizeof(Vertex) * sMaxVertices);
    mVertexStream.setUpStream(0, agl::VertexStreamFormat::RGB32F, offsetof(Vertex, position));

    mVertexAttribute.create(1, nullptr);
    mVertexAttribute.setVertexStream(0, &mVertexStream, 0);
    mVertexAttribute.setUp();

    // Uniform Block
    mUniformBlock = al::createUniformBlock(Vertex::sLayout.data(), Vertex::sLayout.size(), nullptr, 2);

    // Execute List
    ExecuteHelper::instance()->appendDrawTable("３Ｄ（フォワード）", &collisionOrder);
    ExecuteHelper::instance()->addCustomDrawList(
        "ActorDrawCollision[LunaKit]",
        [](const al::ExecuteOrder* order) -> al::ExecutorListBase* {
            Logger::log("Created custom executor list for '%s' with %d", order->listName, order->listMaxSize);
            return new al::ExecutorListActorDraw(order->listName, order->listMaxSize);
        },
        ExecuteHelper::DrawListType::Actor);
}

void CollisionDrawer::draw(agl::DrawContext* context, const al::Scene* scene) const {
    {
        al::UniformBlockSetter ubs{mUniformBlock, 0};

        const sead::Matrix34f viewMtx = al::getViewMtx(scene, 0);
        const sead::Matrix44f projMtx = al::getProjectionMtx(scene, 0);
        sead::Matrix44f projViewMtx;
        projViewMtx.setMul(projMtx, viewMtx);
        mUniformBlock->setData(0, projViewMtx);
    }

    mProgram->activate(context, false);
    mUniformBlock->setUniform(context, agl::UniformBlockLocation("Context", *mProgram));
    mVertexAttribute.activate(context);
    mIndexStream.drawElements(context);
}

void CollisionDrawer::genCube(const sead::Vector3f& pos, const f32 size) {
    auto* vtx = mVertexData.data();
    auto* idx = mIndexData.data();

    {
        mVertexCount = 0;

        // Front
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(+size, +size, +size));
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(+size, -size, +size));
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(-size, -size, +size));
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(-size, +size, +size));

        // Back
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(+size, +size, -size));
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(+size, -size, -size));
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(-size, -size, -size));
        vtx[mVertexCount++] = Vertex(pos + sead::Vector3f(-size, +size, -size));
    }
    {
        mIndexCount = 0;

        // Front
        idx[mIndexCount++] = 0;
        idx[mIndexCount++] = 1;
        idx[mIndexCount++] = 2;

        idx[mIndexCount++] = 2;
        idx[mIndexCount++] = 3;
        idx[mIndexCount++] = 0;

        // Back
        idx[mIndexCount++] = 7;
        idx[mIndexCount++] = 6;
        idx[mIndexCount++] = 5;

        idx[mIndexCount++] = 5;
        idx[mIndexCount++] = 4;
        idx[mIndexCount++] = 7;

        // Left
        idx[mIndexCount++] = 4;
        idx[mIndexCount++] = 5;
        idx[mIndexCount++] = 1;

        idx[mIndexCount++] = 1;
        idx[mIndexCount++] = 0;
        idx[mIndexCount++] = 4;

        // Right
        idx[mIndexCount++] = 3;
        idx[mIndexCount++] = 2;
        idx[mIndexCount++] = 6;

        idx[mIndexCount++] = 6;
        idx[mIndexCount++] = 7;
        idx[mIndexCount++] = 3;

        // Top
        idx[mIndexCount++] = 4;
        idx[mIndexCount++] = 0;
        idx[mIndexCount++] = 3;

        idx[mIndexCount++] = 3;
        idx[mIndexCount++] = 7;
        idx[mIndexCount++] = 4;

        // Bottom
        idx[mIndexCount++] = 6;
        idx[mIndexCount++] = 2;
        idx[mIndexCount++] = 1;

        idx[mIndexCount++] = 1;
        idx[mIndexCount++] = 5;
        idx[mIndexCount++] = 6;

        mIndexStream.setUpStream<u16>(mIndexData, mIndexCount);
    }
}
