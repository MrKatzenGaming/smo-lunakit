#pragma once

#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/Nerve/NerveExecutor.h"

#include "ReplayFrame.h"

class GhostManager;
class PuppetActor;

class Ghost : public al::NerveExecutor {
public:
    Ghost();
    ~Ghost();
    void init(const al::ActorInitInfo& info);
    void removePuppet();
    void startReplay(ReplayFrame* frames, s32 frameCount);
    void endReplay();
    bool isRunning();
    void exeWait();
    void exePlay();

private:
    ReplayFrame* mFrames = nullptr;
    s32 mFrameCount = 0;
    GhostManager* mParent = nullptr;
    PuppetActor* mPuppet = nullptr;
};
