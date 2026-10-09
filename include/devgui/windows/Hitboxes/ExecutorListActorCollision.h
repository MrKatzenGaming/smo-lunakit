#pragma once

#include "al/Library/Execute/ExecutorListActorExecute.h"

namespace al {
struct ExecuteSystemInitInfo;
class ExecutorActorExecuteBase;
}  // namespace al

namespace agl {
class DrawContext;
}  // namespace agl

class ExecutorListActorCollision : public al::ExecutorListActorExecuteBase {
public:
    static constexpr auto sGroupName = "ActorDrawCollision[LunaKit]";

    ExecutorListActorCollision(const char* name, s32 size, const al::ExecuteSystemInitInfo& initInfo);
    al::ExecutorActorExecuteBase* createExecutor(const char* name) const override;

    static void installHooks();

private:
    agl::DrawContext* mContext;
};

class ExecutorListActorCollisionDepthOnly : public al::ExecutorListActorExecuteBase {
public:
    static constexpr auto sGroupName = "ActorDrawCollisionDepthOnly[LunaKit]";

    ExecutorListActorCollisionDepthOnly(const char* name, s32 size, const al::ExecuteSystemInitInfo& initInfo);
    al::ExecutorActorExecuteBase* createExecutor(const char* name) const override;

private:
    agl::DrawContext* mContext;
};
