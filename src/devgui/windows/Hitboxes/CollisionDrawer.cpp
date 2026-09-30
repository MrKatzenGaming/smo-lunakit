#include "devgui/windows/Hitboxes/CollisionDrawer.h"

#include "hk/hook/Trampoline.h"

#include "sead/gfx/seadCamera.h"
#include "sead/gfx/seadGraphics.h"
#include "sead/gfx/seadProjection.h"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"

#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/Execute/ActorExecuteInfo.h"
#include "al/Library/Execute/ExecuteDirector.h"
#include "al/Library/Execute/ExecuteTableHolderDraw.h"
#include "al/Library/Execute/ExecutorListActorDraw.h"
#include "al/Library/Execute/ExecutorListActorModelDraw.h"
#include "al/Library/Resource/Resource.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Shader/Block/UniformBlock.h"

#include "agl/common/aglResBinaryShaderArchive.h"
#include "agl/common/aglResShaderArchive.h"
#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglShaderProgramArchive.h"
#include "agl/common/aglVertexBuffer.h"

#include "custom/al/actor/ActorActionKeeper.h"

#include "helpers/ExecuteHelper.h"
#include "logger/Logger.hpp"

static constexpr auto collisionListName = "Collision[LunaKit]";
static constexpr auto collisionGroupName = "ActorDrawCollision[LunaKit]";
static constexpr al::ExecuteOrder collisionOrder = {collisionListName, collisionGroupName, 1024, "地形"};

ExecutorListActorCollision::ExecutorListActorCollision(const char* name, const s32 size) : ExecutorListActorExecuteBase(name, size) {}

al::ExecutorActorExecuteBase* ExecutorListActorCollision::createExecutor(const char* name) const {
    Logger::log("Creating ExecutorActorCollision for '%s' [%d/%d]", name, mSize, mCapacity);
    return new ExecutorActorCollision(name);
}

HkTrampoline hookInitActorImpl = [](TrampolineStatic(), al::LiveActor* actor, const al::ActorInitInfo& initInfo, const sead::SafeString& folderName,
                                    const sead::SafeString& fileName, const char* suffix) -> void {
    orig(actor, initInfo, folderName, fileName, suffix);
    // al::initExecutorDraw(actor, initInfo, collisionListName);

    // Cannot just call registerActorDraw(), since we don't want to register an actor model
    auto* director = initInfo.executeDirector;

    if (!actor->getExecuteInfo())
        actor->initExecuteInfo(new al::ActorExecuteInfo(director->mRequestKeeper));

    for (s32 i = 0; i < director->mDrawTableCount; i++) {
        // Logger::log(" - #%d: %p", i, director->mDrawTables[i]);
        // for (s32 j = 0; j < director->mDrawTables[i]->mExecutorsActorSize; j++) {
        //     auto* listActor = director->mDrawTables[i]->mExecutorsActor[j];
        //     Logger::log(" - Actor #%d/%d: %p", i, j, listActor);
        // }
        // for (s32 j = 0; j < director->mDrawTables[i]->mExecutorsActorModelSize; j++) {
        //     auto* listActor = director->mDrawTables[i]->mExecutorsActorModel[j];
        //     Logger::log(" - ActorModel #%d/%d: %p", i, j, listActor);
        //     for (s32 k = 0; k < director->mDrawTables[i]->mExecutorsActorModel[j]->mSize; k++) {
        //         auto* model = director->mDrawTables[i]->mExecutorsActorModel[j]->mList[k];
        //         Logger::log(" - Model #%d/%d/%d: %p", i, j, k, model);
        //     }
        // }
        // Logger::log("Registering with %d updaters, %d drawers", actor->getExecuteInfo()->getUpdaterCount(),
        //             actor->getExecuteInfo()->getDrawerCount());

        director->mDrawTables[i]->tryRegisterActor(actor, collisionListName);
    }
};

void ExecutorListActorCollision::init() {
    hookInitActorImpl.installAtSym<"ActorInitUtil_initActorImpl">();
}

ExecutorActorCollision::ExecutorActorCollision(const char* name) : ExecutorActorExecuteBase(name) {}

void ExecutorActorCollision::execute() const {
    Logger::log("Executing ExecutorActorCollision for '%s' with %d actors", mName, mActors.size());
}

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
        collisionGroupName,
        [](const al::ExecuteOrder* order) -> al::ExecutorListBase* {
            Logger::log("Created custom executor list for '%s' with %d slots", order->listName, order->listMaxSize);
            return new ExecutorListActorCollision(order->listName, order->listMaxSize);
        },
        ExecuteHelper::DrawListType::Actor);

    ExecutorListActorCollision::init();
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
