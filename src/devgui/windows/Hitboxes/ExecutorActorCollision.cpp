#include "devgui/windows/Hitboxes/ExecutorActorCollision.h"

#include "hk/hook/Trampoline.h"

#include "sead/gfx/seadGraphics.h"

#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/Collision/CollisionParts.h"
#include "al/Library/Collision/CollisionPartsTriangle.h"
#include "al/Library/Draw/GraphicsSystemInfo.h"
#include "al/Library/Execute/ExecutorActorExecuteBase.h"
#include "al/Library/LiveActor/ActorFlagFunction.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/Model/SimpleModelEnv.h"

#include "agl/common/aglShaderProgram.h"

#include "helpers/ShaderHelper.h"

ExecutorActorCollision::ExecutorActorCollision(const char* name, agl::DrawContext* context, bool depthOnly)
    : ExecutorActorExecuteBase(name), mContext(context) {
    mShader = SEAD_INSTANCE(ShaderHelper)->getShaderProgram(depthOnly ? "lkRenderCollisionDepthOnly" : "lkRenderCollision");
}

void ExecutorActorCollision::execute() const {
    for (s32 i = 0; i < mActors.size(); i++) {
        const auto* actor = mActors[i];
        if (!actor || al::isDead(actor))
            continue;

        const auto* parts = actor->getCollisionParts();
        if (!parts)
            continue;

        const auto* server = reinterpret_cast<RenderedKCollisionServer*>(parts->getKCollisionServer());
        if (!server)
            continue;

        server->draw(mContext, actor, mShader);
    }
}

RenderedKCollisionServer::RenderedKCollisionServer() {}

void RenderedKCollisionServer::initRender() {
    u32 vertexMaxNum = 0;

    for (const auto& model : mModelsData)
        vertexMaxNum += getTriangleNum(&model) * 3;

    // Buffers
    mVertexData.alloc(vertexMaxNum, nullptr);

    // Vertex Attribute
    mVertexStream.setUpBuffer(mVertexData, sizeof(Vertex), sizeof(Vertex) * vertexMaxNum);
    mVertexStream.setUpStream(0, agl::VertexStreamFormat::RGB32F, offsetof(Vertex, position));
    mVertexStream.setUpStream(1, agl::VertexStreamFormat::RGB32F, offsetof(Vertex, normal));

    mVertexAttribute.create(1, nullptr);
    mVertexAttribute.setVertexStream(0, &mVertexStream, 0);
    mVertexAttribute.setVertexStream(1, &mVertexStream, 1);
    mVertexAttribute.setUp();

    // Uniform Block
    mUniformBlock = al::createUniformBlock(sUniformLayout.data(), sUniformLayout.size(), nullptr, 2);

    // Data
    auto* vtx = mVertexData.data();
    mVertexNum = 0;

    for (const auto& model : mModelsData) {
        for (s32 i = 0; i < getTriangleNum(&model); i++) {
            const auto& data = getPrismData(i, &model);

            sead::Vector3f positions[3];
            calcPosLocal(&positions[0], &data, 0, &model);
            calcPosLocal(&positions[1], &data, 1, &model);
            calcPosLocal(&positions[2], &data, 2, &model);

            sead::Vector3f faceNormal = getFaceNormal(&data, &model);

            // Nudge slightly to avoid Z-fighting
            constexpr f32 nudge = 1.0f;
            positions[0] += faceNormal * nudge;
            positions[1] += faceNormal * nudge;
            positions[2] += faceNormal * nudge;

            vtx[mVertexNum++] = Vertex(positions[0], faceNormal);
            vtx[mVertexNum++] = Vertex(positions[1], faceNormal);
            vtx[mVertexNum++] = Vertex(positions[2], faceNormal);
        }
    }
}

void RenderedKCollisionServer::draw(agl::DrawContext* context, const al::LiveActor* actor, const agl::ShaderProgram* shader) const {
    {
        al::UniformBlockSetter ubs{mUniformBlock, 0};
        mUniformBlock->setData(0, actor->getCollisionParts()->getBaseMtx());
    }

    shader->activate(context, false);
    mUniformBlock->setUniform(context, agl::UniformBlockLocation("Mdl", *shader));
    mVertexAttribute.drawArrays(context, VertexStreamTopology::Triangles, 0, mVertexNum);
}

static void RenderedKCollisionServer_ctor(RenderedKCollisionServer* self) {
    new (self) RenderedKCollisionServer();
}

HkTrampoline hookKCollisionServerInit = [](TrampolineStatic(), RenderedKCollisionServer* self, void* kcl, const void* byml) -> void {
    orig(self, kcl, byml);
    self->initRender();
};

void RenderedKCollisionServer::installHooks() {
    hk::hook::a64::assemble<"mov x0, {}">().arg(sizeof(RenderedKCollisionServer)).installAtSym<"$kCollisionServerSize">();
    hk::hook::writeBranchLinkAtSym<"$kCollisionServerCtor">(&RenderedKCollisionServer_ctor);
    hookKCollisionServerInit.installAtSym<"_ZN2al16KCollisionServer20initKCollisionServerEPvPKv">();
}
