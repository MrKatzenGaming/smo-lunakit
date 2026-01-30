#pragma once

#include "al/actor/ActorInitInfo.h"
#include "al/actor/Placement.h"
#include "al/scene/SceneObjHolder.h"

#include "math/seadVector.h"
#include "prim/seadSafeString.h"

class QuestInfo : public al::IUseSceneObjHolder {
public:
    QuestInfo(void);

    virtual al::SceneObjHolder* getSceneObjHolder(void) const override { return mSceneObjHolder; };

    void setStageName(const char*);
    void setLabel(const char*);
    void isEqual(const QuestInfo*);

    void init(const al::PlacementInfo&, al::SceneObjHolder*);
    void init(const al::PlacementInfo&, const al::ActorInitInfo&);
    void init(const al::ActorInitInfo&);

    void end(void);
    void copy(const QuestInfo*);
    void clear(void);

    int mQuestID = -1;                                  // 0x8
    sead::Vector3f mShineTrans = sead::Vector3f::zero;  // 0xC
    bool mIsMainQuest;                                  // 0x18
    al::SceneObjHolder* mSceneObjHolder;                // 0x20
    sead::FixedSafeString<0x80> mLabel;                 // 0x28
    sead::FixedSafeString<0x80> mStageName;             // 0xC0
    bool mIsSingle;                                     // 0x158
    sead::FixedSafeString<0x80> mObjID;                 // 0x160
    sead::FixedSafeString<0x80> mPlacementStageName;    // 0x1F8
};

static_assert(sizeof(QuestInfo) == 0x290, "Quest Info Size");
