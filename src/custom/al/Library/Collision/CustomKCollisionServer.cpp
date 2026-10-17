#include "custom/al/Library/Collision/CustomKCollisionServer.h"

#include "hk/hook/Trampoline.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Collision/CollisionDirector.h"
#include "al/Library/Collision/CollisionParts.h"
#include "al/Library/Collision/CollisionPartsKeeperUtil.h"
#include "al/Library/Collision/KCollisionServer.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglGPUMemBlock.h"
#include "agl/common/aglIndexStream.h"
#include "agl/common/aglShaderLocation.h"
#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglShaderProgramArchive.h"
#include "agl/common/aglVertexAttribute.h"
#include "agl/common/aglVertexBuffer.h"

#include "helpers/ShaderHelper.h"
#include "math/seadVectorCalcCommon.h"

namespace al {

CustomKCollisionServer::CustomKCollisionServer() {}

void CustomKCollisionServer::initRender() {
    u32 vertexMaxNum = 0;

    for (const auto& model : mModelsData)
        vertexMaxNum += getTriangleNum(&model) * 3;

    // Buffers
    auto vtxData = mVertexData.alloc(vertexMaxNum, nullptr);

    // Vertex Attribute
    mVertexStream.setUpBuffer(mVertexData, sizeof(Vertex), sizeof(Vertex) * vertexMaxNum);
    mVertexStream.setUpStream(0, agl::VertexStreamFormat::RGB32F, offsetof(Vertex, position));
    mVertexStream.setUpStream(1, agl::VertexStreamFormat::RGB32F, offsetof(Vertex, normal));
    mVertexStream.setUpStream(2, agl::VertexStreamFormat::R32F, offsetof(Vertex, edgeDistance));

    mVertexAttribute.create(1, nullptr);
    mVertexAttribute.setVertexStream(0, &mVertexStream, 0);
    mVertexAttribute.setVertexStream(1, &mVertexStream, 1);
    mVertexAttribute.setVertexStream(2, &mVertexStream, 2);
    mVertexAttribute.setUp();

    // Uniform Block
    mUniformBlock = al::createUniformBlock(sUniformLayout.data(), sUniformLayout.size(), nullptr, 2);

    // Data
    auto* vtx = vtxData.getData();
    mVertexNum = 0;

    for (const auto& model : mModelsData) {
        for (s32 i = 0; i < getTriangleNum(&model); i++) {
            const auto& data = getPrismData(i, &model);

            auto& origin = getVertexData(data.posIndex, &model);
            auto& faceNormal = getFaceNormal(&data, &model);
            auto& edgeNormal1 = getEdgeNormal1(&data, &model);
            auto& edgeNormal2 = getEdgeNormal2(&data, &model);
            auto& edgeNormal3 = getEdgeNormal3(&data, &model);

            sead::Vector3f cross1, cross2, cross3;
            cross1.setCross(edgeNormal1, faceNormal);
            cross2.setCross(edgeNormal2, faceNormal);
            cross3.setCross(edgeNormal3, faceNormal);

            sead::Vector3f posA = origin;
            sead::Vector3f posB = origin + cross2 * (data.length / cross2.dot(edgeNormal3));
            sead::Vector3f posC = origin + cross1 * (data.length / cross1.dot(edgeNormal3));

            f32 edgeDistA = (posA - posB).cross(cross3).length();
            f32 edgeDistB = (posB - posC).cross(cross1).length();
            f32 edgeDistC = (posC - posA).cross(cross2).length();

            // Nudge slightly to avoid Z-fighting
            constexpr f32 nudge = 1.0f;
            vtx[mVertexNum++] = Vertex(posA + faceNormal * nudge, faceNormal, edgeDistA);
            vtx[mVertexNum++] = Vertex(posB + faceNormal * nudge, faceNormal, edgeDistB);
            vtx[mVertexNum++] = Vertex(posC + faceNormal * nudge, faceNormal, edgeDistC);
        }
    }
}

void CustomKCollisionServer::draw(agl::DrawContext* context, const agl::ShaderProgram* shader, const al::CollisionParts* parts) const {
    {
        al::UniformBlockSetter ubs{mUniformBlock, 0};
        mUniformBlock->setDataMat3x4(0, &parts->getBaseMtx());
    }

    shader->activate(context, false);
    mUniformBlock->activate(context, agl::UniformBlockLocation("Mdl", *shader));
    mVertexAttribute.drawArrays(context, VertexStreamTopology::Triangles, 0, mVertexNum);
}

static void CustomKCollisionServer_ctor(CustomKCollisionServer* self) {
    new (self) CustomKCollisionServer();
}

HkTrampoline hookKCollisionServerInit = [](TrampolineStatic(), CustomKCollisionServer* self, void* kcl, const void* byml) -> void {
    orig(self, kcl, byml);
    self->initRender();
};

void CustomKCollisionServer::installHooks() {
    hk::hook::a64::assemble<"mov x0, {}">().arg(sizeof(CustomKCollisionServer)).installAtSym<"$kCollisionServerSize">();
    hk::hook::writeBranchLinkAtSym<"$kCollisionServerCtor">(&CustomKCollisionServer_ctor);
    hookKCollisionServerInit.installAtSym<"_ZN2al16KCollisionServer20initKCollisionServerEPvPKv">();
}

}  // namespace al
