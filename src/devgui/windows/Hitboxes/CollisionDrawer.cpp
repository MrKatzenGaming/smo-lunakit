#include "devgui/windows/Hitboxes/CollisionDrawer.h"

#include "hk/hook/Trampoline.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Collision/CollisionDirector.h"
#include "al/Library/Collision/CollisionPartsKeeperOctree.h"
#include "al/Library/Collision/CollisionPartsKeeperUtil.h"
#include "al/Library/Execute/ExecuteUtil.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Shader/Block/UniformBlock.h"
#include "al/Library/Thread/FunctorV0M.h"

#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglVertexBuffer.h"

#include "helpers/ExecuteHelper.h"
#include "helpers/ShaderHelper.h"
#include "logger/Logger.hpp"

SEAD_SINGLETON_DISPOSER_IMPL(CollisionDrawer);

static constexpr al::ExecuteOrder collisionOrder = {"Collision[LunaKit]", "Functor", 1, "地形"};
static constexpr al::ExecuteOrder collisionOrderDepthOnly = {"CollisionDepthOnly[LunaKit]", "Functor", 1, "地形"};

HkTrampoline hookEndInit = [](TrampolineStatic(), al::LiveActorKit* self) -> void {
    SEAD_INSTANCE(CollisionDrawer)->initScene(self);
    orig(self);
};

CollisionDrawer::CollisionDrawer() {
    ExecuteHelper::instance()->appendDrawTable("３Ｄ（ディファード地形）", &collisionOrder);
    ExecuteHelper::instance()->appendDrawTable("３Ｄ（不透明Ｚプリパス）", &collisionOrderDepthOnly);

    mShader = SEAD_INSTANCE(ShaderHelper)->getShaderProgram("lkRenderCollision");
    mShaderDepthOnly = SEAD_INSTANCE(ShaderHelper)->getShaderProgram("lkRenderCollisionDepthOnly");

    hookEndInit.installAtSym<"_ZN2al12LiveActorKit7endInitEv">();

    RenderedKCollisionServer::installHooks();
}

CollisionDrawer::~CollisionDrawer() = default;

void CollisionDrawer::initScene(const al::LiveActorKit* actorKit) {
    using CollisionDrawerFunctor = al::FunctorV0M<CollisionDrawer*, void (CollisionDrawer::*)() const>;

    al::registerExecutorFunctor(collisionOrder.listName, actorKit->getExecuteDirector(), CollisionDrawerFunctor(this, &CollisionDrawer::draw));
    al::registerExecutorFunctor(collisionOrderDepthOnly.listName, actorKit->getExecuteDirector(),
                                CollisionDrawerFunctor(this, &CollisionDrawer::drawDepthOnly));

    mCollisionDirector = actorKit->getCollisionDirector();
    mDrawContext = actorKit->getSceneDrawContext();
    Logger::log("Initialized CollisionDrawer with '%p': %p / %p", actorKit, mCollisionDirector, mDrawContext);
}

static void drawNode(agl::DrawContext* context, const agl::ShaderProgram* shader, const al::CollisionPartsKeeperOctree::OctNode* node) {
    for (const auto parts : node->collisionParts) {
        if (parts->mSpecialPurpose != nullptr)
            continue;

        const auto server = reinterpret_cast<RenderedKCollisionServer*>(parts->getKCollisionServer());
        server->draw(context, shader, parts);
    }

    for (s32 i = 0; i < 8; i++)
        if (node->octants[i])
            drawNode(context, shader, node->octants[i]);
}

void CollisionDrawer::drawImpl(const bool depthOnly) const {
    if (!mCollisionDirector || !mDrawContext) {
        Logger::log("Uninitialized skip");
        return;
    }

    auto octree = mCollisionDirector->mRootOctree;
    drawNode(mDrawContext, depthOnly ? mShaderDepthOnly : mShader, &octree->mRoot);
}

RenderedKCollisionServer::RenderedKCollisionServer() {}

void RenderedKCollisionServer::initRender() {
    u32 vertexMaxNum = 0;

    for (const auto& model : mModelsData)
        vertexMaxNum += getTriangleNum(&model) * 3;

    // Buffers
    auto vtxData = mVertexData.alloc(vertexMaxNum, nullptr);

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
    auto* vtx = vtxData.getData();
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

void RenderedKCollisionServer::draw(agl::DrawContext* context, const agl::ShaderProgram* shader, const al::CollisionParts* parts) const {
    {
        al::UniformBlockSetter ubs{mUniformBlock, 0};
        mUniformBlock->setDataMat3x4(0, &parts->getBaseMtx());
    }

    shader->activate(context, false);
    mUniformBlock->activate(context, agl::UniformBlockLocation("Mdl", *shader));
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
