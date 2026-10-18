#pragma once

#include "sead/basis/seadTypes.h"

#include "al/Library/Execute/ActorExecuteInfo.h"

namespace al {
class ExecuteRequestKeeper;
class ExecutorActorExecuteBase;
class ModelDrawerBase;

class CustomActorExecuteInfo {
public:
    void ctor(ExecuteRequestKeeper* keeper);
    void addUpdater(ExecutorActorExecuteBase* updater);
    void addDrawer(ModelDrawerBase* drawer);

    static void installHooks();

private:
    ExecuteRequestKeeper* mRequestKeeper = nullptr;
    s32 mUpdaterCount = 0;

    union {
        // Custom
        struct {
            ExecutorActorExecuteBase** mCustomUpdaters = nullptr;
            s32 mCustomUpdaterCapacity = 0;
        };

        // Vanilla
        ExecutorActorExecuteBase* mVanillaUpdaters[4];
    };

    s32 mDrawerCount = 0;

    union {
        // Custom
        struct {
            ModelDrawerBase** mCustomDrawers = nullptr;
            s32 mCustomDrawerCapacity = 0;
        };

        // Vanilla
        ModelDrawerBase* mVanillaDrawers[11];
    };
};

static_assert(sizeof(CustomActorExecuteInfo) == sizeof(ActorExecuteInfo));

}  // namespace al
