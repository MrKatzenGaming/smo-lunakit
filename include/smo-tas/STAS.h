#pragma once

#include "hk/Result.h"

#include "nn/fs/fs_types.h"
#include "nn/types.h"

#include "Library/Nerve/NerveExecutor.h"
#include "Library/Scene/Scene.h"

#include <cstddef>

#include "Script.h"
#include "container/seadSafeArray.h"

#define TAS_SCRIPTPATH "sd:/smo/tas/scripts"

class STAS : public al::NerveExecutor {
    SEAD_SINGLETON_DISPOSER(STAS);
    STAS();
    ~STAS() = default;

public:
    static void installHooks();

    void endScript();
    void applyCommand(Command* cmd);
    void exeUpdate();
    void exeWait() {};
    void updateDir();
    hk::Result tryStartScript();
    hk::Result tryLoadScript();
    bool isRunning();

    void setScene(al::Scene* scene) { mScene = scene; };

    void setScript(nn::fs::DirectoryEntry entry) { mLoadedEntry = entry; };

    const char* getScriptName() const { return mLoadedEntry.mName; };

    nn::fs::DirectoryEntry* getScripts() const { return mEntries; };

    s64 getEntryCount() const { return mEntryCount; };

    s64 getCurFrame() const { return mCurFrame; };

    u32 getFrameCount() const { return mScript->getFrames(); };

    Script* getScript() { return mScript; };

    bool hasScript() const { return mScript != nullptr; };

public:
    u32 mSpeed = 1;
    u32 mSpeedupFrame = 0;
    bool mIsSpeedupDone = false;

    bool mIsUseAbsJoystick = false;

private:
    s64 mCurFrame = -1;
    s64 mNextFrame = -1;

    sead::SafeArray<u64, 2> mPrevButtons = {0, 0};

    s64 mEntryCount = 0;
    nn::fs::DirectoryEntry mLoadedEntry;
    nn::fs::DirectoryEntry* mEntries = nullptr;

    Script* mScript = nullptr;

    al::Scene* mScene = nullptr;
};
