#pragma once

#include "nn/fs/fs_types.h"

#include "Library/Nerve/NerveExecutor.h"
#include "Library/Scene/Scene.h"

#include "InputFrame.h"
#include "heap/seadDisposer.h"

#define TAS_SCRIPTPATH "sd:/smo/tas/scripts"

class TAS : public al::NerveExecutor {
    SEAD_SINGLETON_DISPOSER(TAS);
    TAS();
    ~TAS();

public:
    bool tryLoadScript();
    void startScript();
    void endScript();
    void applyFrame(InputFrame& frame);
    void exeUpdate();
    void exeWait();
    void exeWaitUpdate();
    void exeRecord();
    bool isRunning();

    void setScene(al::Scene* scene) { mScene = scene; };

    void setScript(nn::fs::DirectoryEntry entry) { mLoadedEntry = entry; };

    al::Scene* getScene() const { return mScene; };

    const char* getScriptName() const { return mLoadedEntry.mName; };

    nn::fs::DirectoryEntry* getScripts() const { return mEntries; };

    s64 getEntryCount() const { return mEntryCount; };

    u32 getFrameIndex() const { return mFrameIndex; };

    u32 getFrameCount() const { return mScript->mFrameCount; };

    OScript* getScript() { return mScript; };

    sead::Vector3f getStartPosition() const { return mScript->mStartPosition; };

    bool hasScript() const { return mScript != nullptr; };

    void updateDir();
    bool tryStartScript();

    bool isUseAbsoluteJoystick() { return mIsUseAbsoluteJoystick; };

    bool* isUseAbsoluteJoystickPtr() { return &mIsUseAbsoluteJoystick; };

    void setIsUseAbsoluteJoystick(bool use) { mIsUseAbsoluteJoystick = use; };

    int* getSpeedPtr() { return &mSpeed; };

    int getSpeed() { return mSpeed; };

    void setSpeed(int speed) { mSpeed = speed; };

    int* getSpeedUntilFramePtr() { return &mSpeedUntilFrame; };

    int getSpeedUntilFrame() { return mSpeedUntilFrame; };

    bool hasSpeedUntilFrame() { return mHasSpeedUntilFrame; };

    void setHasSpeedUntilFrame(bool has) { mHasSpeedUntilFrame = has; };

private:
    s64 mEntryCount;
    nn::fs::DirectoryEntry* mEntries = nullptr;
    nn::fs::DirectoryEntry mLoadedEntry;
    u32 mFrameIndex = 0;
    u32 mPrevButtons[2];
    OScript* mScript = nullptr;
    al::Scene* mScene = nullptr;
    bool mIsUseAbsoluteJoystick = false;
    int mSpeed = 1;
    int mSpeedUntilFrame = 0;
    bool mHasSpeedUntilFrame = false;
};
