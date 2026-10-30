#pragma once

#include "Library/LiveActor/LiveActor.h"

#include "container/seadPtrArray.h"

namespace al {
class FixMapParts;
class LiveActorGroup;
}  // namespace al

class FukuwaraiFaceParts;
class FukuwaraiNpc;

class FukuwaraiWatcher : al::LiveActor {
public:
    FukuwaraiWatcher(const char* name);

    void init(const al::ActorInitInfo& initInfo) override;
    void control() override;

    s32 getPartsNum() const;
    FukuwaraiFaceParts* getParts(s32 index) const;

    void exeWait();
    void exeSetStartPosition();
    void exeMemorize();
    void exePlay();
    void exeWaitStartResultEnd();

    s32 calcTotalScore() const;

    void exeResultWait();
    void exeResultAppearParts();
    void exeEnd();

public:
    al::LiveActorGroup* mFacePartGroup;
    sead::FixedPtrArray<FukuwaraiFaceParts, 30> mPlacedParts;

    FukuwaraiNpc* mNpc;
    al::FixMapParts* mFace;
    al::FixMapParts* mFaceLine;
    al::FixMapParts* mFaceSilhouette;

    bool mIsMario;
};

static_assert(sizeof(FukuwaraiWatcher) == 0x238);
