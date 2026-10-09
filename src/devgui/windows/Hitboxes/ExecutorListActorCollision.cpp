#include "devgui/windows/Hitboxes/ExecutorListActorCollision.h"

#include "hk/hook/Trampoline.h"

#include "al/Library/Execute/ActorExecuteInfo.h"
#include "al/Library/Execute/ExecuteDirector.h"
#include "al/Library/Execute/ExecuteTableHolderDraw.h"
#include "al/Library/LiveActor/ActorInitInfo.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Project/Execute/ExecuteSystemInitInfo.h"

#include "devgui/windows/Hitboxes/ExecutorActorCollision.h"
#include "helpers/ExecuteHelper.h"

static constexpr auto collisionListName = "Collision[LunaKit]";
static constexpr al::ExecuteOrder collisionOrder = {collisionListName, ExecutorListActorCollision::sGroupName, 1024, "地形"};
static constexpr al::ExecuteOrder collisionOrderDepthOnly = {collisionListName, ExecutorListActorCollisionDepthOnly::sGroupName, 1024, "地形"};

ExecutorListActorCollision::ExecutorListActorCollision(const char* name, const s32 size, const al::ExecuteSystemInitInfo& initInfo)
    : ExecutorListActorExecuteBase(name, size), mContext(initInfo.drawCtx) {}

al::ExecutorActorExecuteBase* ExecutorListActorCollision::createExecutor(const char* name) const {
    return new ExecutorActorCollision(name, mContext, false);
}

ExecutorListActorCollisionDepthOnly::ExecutorListActorCollisionDepthOnly(const char* name, const s32 size, const al::ExecuteSystemInitInfo& initInfo)
    : ExecutorListActorExecuteBase(name, size), mContext(initInfo.drawCtx) {}

al::ExecutorActorExecuteBase* ExecutorListActorCollisionDepthOnly::createExecutor(const char* name) const {
    return new ExecutorActorCollision(name, mContext, true);
}

HkTrampoline hookInitActorImpl = [](TrampolineStatic(), al::LiveActor* actor, const al::ActorInitInfo& initInfo, const sead::SafeString& folderName,
                                    const sead::SafeString& fileName, const char* suffix) -> void {
    orig(actor, initInfo, folderName, fileName, suffix);

    // Cannot just call registerActorDraw(), since we don't want to register an actor model
    auto* director = initInfo.executeDirector;

    if (!actor->getExecuteInfo())
        actor->initExecuteInfo(new al::ActorExecuteInfo(director->mRequestKeeper));

    for (s32 i = 0; i < director->mDrawTableCount; i++)
        director->mDrawTables[i]->tryRegisterActor(actor, collisionListName);
};

void ExecutorListActorCollision::installHooks() {
    hookInitActorImpl.installAtSym<"$initActorImpl">();

    ExecuteHelper::instance()->appendDrawTable("３Ｄ（ディファード地形）", &collisionOrder);
    ExecuteHelper::instance()->appendDrawTable("３Ｄ（不透明Ｚプリパス）", &collisionOrderDepthOnly);

    ExecuteHelper::instance()->addCustomDrawList(
        ExecutorListActorCollision::sGroupName,
        [](const al::ExecuteOrder* order, const al::ExecuteSystemInitInfo& initInfo) -> ExecutorListBase* {
            return new ExecutorListActorCollision(order->listName, order->listMaxSize, initInfo);
        },
        ExecuteHelper::DrawListType::Actor);
    ExecuteHelper::instance()->addCustomDrawList(
        ExecutorListActorCollisionDepthOnly::sGroupName,
        [](const al::ExecuteOrder* order, const al::ExecuteSystemInitInfo& initInfo) -> ExecutorListBase* {
            return new ExecutorListActorCollisionDepthOnly(order->listName, order->listMaxSize, initInfo);
        },
        ExecuteHelper::DrawListType::Actor);
}
