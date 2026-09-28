#include "devgui/windows/Hitboxes/CollisionDrawer.h"

#include "nn/gfx/gfx_BufferInfo.h"
#include "nn/gfx/gfx_CommandBuffer.h"
#include "nn/gfx/gfx_Enum.h"
#include "nn/gfx/gfx_GpuAddress.h"
#include "nn/gfx/gfx_MemoryPoolInfo.h"
#include "nn/gfx/gfx_Types.h"

#include "sead/gfx/seadCamera.h"
#include "sead/gfx/seadGraphics.h"
#include "sead/gfx/seadProjection.h"
#include "sead/math/seadVector.h"
#include "sead/prim/seadPtrUtil.h"
#include "sead/prim/seadSafeString.h"

#include "Library/Camera/CameraUtil.h"
#include "Library/File/FileUtil.h"
#include "Library/Scene/Scene.h"
#include "Library/Shader/Block/UniformBlock.h"
#include "al/Library/Base/StringUtil.h"
#include "al/Library/Resource/Resource.h"
#include "al/Library/Resource/ResourceFunction.h"

#include "agl/common/aglResBinaryShaderArchive.h"
#include "agl/common/aglResShaderArchive.h"
#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglShaderProgramArchive.h"
#include "agl/common/aglVertexBuffer.h"
#include "agl/detail/aglGPUMemBlockMgr.h"
#include "agl/driver/aglGraphicsDriverMgr.h"

#include "logger/Logger.hpp"

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
}

void CollisionDrawer::draw(agl::DrawContext* context, const al::Scene* scene) const {
    {
        al::UniformBlockSetter ubs{mUniformBlock, 0};

        const sead::Matrix34f viewMtx = al::getViewMtx(scene, 0);
        Logger::log("=== View Matrix ===");
        Logger::log("[[%.3f, %.3f, %.3f, %.3f],", viewMtx.m[0][0], viewMtx.m[0][1], viewMtx.m[0][2], viewMtx.m[0][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f],", viewMtx.m[1][0], viewMtx.m[1][1], viewMtx.m[1][2], viewMtx.m[1][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f]]", viewMtx.m[2][0], viewMtx.m[2][1], viewMtx.m[2][2], viewMtx.m[2][3]);
        const sead::Matrix44f projMtx = al::getProjectionMtx(scene, 0);
        Logger::log("=== Proj Matrix ===");
        Logger::log("[[%.3f, %.3f, %.3f, %.3f],", projMtx.m[0][0], projMtx.m[0][1], projMtx.m[0][2], projMtx.m[0][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f],", projMtx.m[1][0], projMtx.m[1][1], projMtx.m[1][2], projMtx.m[1][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f],", projMtx.m[2][0], projMtx.m[2][1], projMtx.m[2][2], projMtx.m[2][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f]]", projMtx.m[3][0], projMtx.m[3][1], projMtx.m[3][2], projMtx.m[3][3]);
        sead::Matrix44f projViewMtx;
        projViewMtx.setMul(projMtx, viewMtx);
        Logger::log("=== ProjView Matrix ===");
        Logger::log("[[%.3f, %.3f, %.3f, %.3f],", projViewMtx.m[0][0], projViewMtx.m[0][1], projViewMtx.m[0][2], projViewMtx.m[0][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f],", projViewMtx.m[1][0], projViewMtx.m[1][1], projViewMtx.m[1][2], projViewMtx.m[1][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f],", projViewMtx.m[2][0], projViewMtx.m[2][1], projViewMtx.m[2][2], projViewMtx.m[2][3]);
        Logger::log(" [%.3f, %.3f, %.3f, %.3f]]", projViewMtx.m[3][0], projViewMtx.m[3][1], projViewMtx.m[3][2], projViewMtx.m[3][3]);
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
