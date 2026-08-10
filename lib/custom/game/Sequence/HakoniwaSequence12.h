#pragma once

#include "sead/basis/seadTypes.h"

#include "al/Library/Sequence/Sequence.h"

#include "game/System/GameDataHolderAccessor.h"

class HakoniwaStateDemoOpening;
class HakoniwaStateDemoEnding;
class HakoniwaStateDemoWorldWarp;
class HakoniwaStateSimpleDemo;
class HakoniwaStateBootLoadData;
class HakoniwaStateDeleteScene;
class HakoniwaStateDeleteSceneWithBalloonNetwork;
class WorldResourceLoader;
class StageScene;
class GameDataHolderAccessor;
class BootLayout;
class TimeBalloonSequenceInfo;
class CollectBgmPlayer;
class LoadLayoutCtrl;

namespace al {
class WipeHolder;
class Scene;
class AudioDirector;
class LayoutKit;
class SimpleLayoutAppearWaitEnd;
class AsyncFunctorThread;
class SeadAudioPlayer;
class AudioBusSendFader;
class SimpleAudioUser;
class ScreenCaptureExecutor;
class GamePadSystem;
class EffectSystem;

}  // namespace al

class HakoniwaSequence12 : public al::Sequence {
public:
    HakoniwaSequence12(const char* name);

    void init(const al::SequenceInitInfo& initInfo) override;
    void update() override;
    void drawMain() const override;

    void updatePadSystem();
    void destroySceneHeap(bool destroyResource);
    void initSystem();
    bool isEnableSave() const;

    void exeBootLoadData();
    void exeDemoOpening();
    void exeLoadWorldResource();
    void exeLoadWorldResourceWithBoot();
    void exeLoadStage();
    void exePlayStage();
    void exeDemoWorldWarp();
    void exeDemoEnding();
    void exeDestroy();
    void exeMiss();
    void exeMissCoinSub();
    void exeMissEnd();
    void exeDemoLava();
    void exeFadeToNewGame();
    void exeChangeLanguage();
    void exeWaitWriteData();
    void exeWaitLoadData();
    void exeWaitWriteDataModeChange();
    void exeWaitLoadDataModeChange();

    bool isDisposable() const override;
    al::Scene* getCurrentScene() const override;

public:
    al::Scene* mCurrentScene;
    GameDataHolderAccessor mGameDataHolderAccessor;
    al::GamePadSystem* mGamePadSystem;
    HakoniwaStateDemoOpening* mStateDemoOpening;
    HakoniwaStateDemoEnding* mStateDemoEnding;
    HakoniwaStateDemoWorldWarp* mStateDemoWorldWarp;
    HakoniwaStateSimpleDemo* mStateSimpleDemo;
    HakoniwaStateBootLoadData* mStateBootLoadData;
    HakoniwaStateDeleteScene* mStateDeleteScene;
    HakoniwaStateDeleteSceneWithBalloonNetwork* mStateDeleteSceneWithBalloonNetwork;
    al::LayoutKit* mLayoutKit;
    bool _100;
    sead::FixedSafeString<128> mStageName;
    s32 mNextScenarioNum;
    s32 _1a4;
    al::ScreenCaptureExecutor* mScreenCaptureExecutor;
    al::WipeHolder* mWipeHolder;
    bool mIsMissEnd;
    al::SimpleLayoutAppearWaitEnd* mCounterMiss;
    s32 mCurrentCoins;
    s32 mFinalCoins;
    BootLayout* mBootLayout;
    al::EffectSystem* mEffectSystem;
    al::AsyncFunctorThread* mInitThread;
    bool mIsInitialized;
    al::SeadAudioPlayer* mSeAudioPlayer;
    al::SeadAudioPlayer* mBgmAudioPlayer;
    al::AudioBusSendFader* mAudioBusSendFader;
    WorldResourceLoader* mResourceLoader;
    sead::Heap* mPlayerResourceHeap;
    sead::FixedSafeString<128> mCapName;
    sead::FixedSafeString<128> mCostumeName;
    al::SimpleAudioUser* mPlayerAudioUser;
    bool mIsHackEnd;
    bool mIsWarpCheckpoint;
    TimeBalloonSequenceInfo* mBalloonSeqInfo;
    CollectBgmPlayer* mCollectBgmPlayer;
    sead::FixedSafeString<128> mLanguage;
    s32 mFileId;
    LoadLayoutCtrl* mLoadLayoutCtrl;
    bool mIsKidsMode;
};

static_assert(sizeof(HakoniwaSequence12) == 0x420, "HakoniwaSequnce size");
