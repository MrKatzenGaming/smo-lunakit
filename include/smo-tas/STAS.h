#pragma once

#include "hk/Result.h"

#include "nn/fs/fs_types.h"

#include "Library/Nerve/NerveExecutor.h"
#include "Library/Scene/Scene.h"

#include <cstddef>

#include "heap/seadDisposer.h"
#include "heap/seadHeap.h"
#include "math/seadVector.h"  // IWYU pragma: keep
#include "math/seadVectorFwd.h"

#define TAS_SCRIPTPATH "sd:/smo/tas/scripts"
#pragma pack(push, 4)

struct FileHeader {
    u8 magic[4] = "";
    u16 version = 0;
    u16 addon_version = 0;
    u16 editor_extras = 0;
    u64 title_id = 0;
};

struct ScriptHeader {
    u32 cmdCount = 0;
    u32 frameCount = 0;
    u32 editingSeconds = 0;
    u8 conTypes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    u16 author_len = 0;
    wchar_t* author = nullptr;
    u32 title_len = 0;
    wchar_t* title = nullptr;
    u32 desc_len = 0;
    wchar_t* desc = nullptr;
    u64 gameHeaderSize = 0;
    u8* gameHeader = nullptr;
};

#pragma pack(pop)

enum Button : s32 {
    cSTAS_A = 0,
    cSTAS_B,
    cSTAS_X,
    cSTAS_Y,
    cSTAS_LeftStick,
    cSTAS_RightStick,
    cSTAS_L,
    cSTAS_R,
    cSTAS_ZL,
    cSTAS_ZR,
    cSTAS_Plus,
    cSTAS_Minus,
    cSTAS_DLeft,
    cSTAS_DUp,
    cSTAS_DRight,
    cSTAS_DDown,
    cSTAS_LeftStickLeft,
    cSTAS_LeftStickUp,
    cSTAS_LeftStickRight,
    cSTAS_LeftStickDown,
    cSTAS_RightStickLeft,
    cSTAS_RightStickUp,
    cSTAS_RightStickRight,
    cSTAS_RightStickDown,
};

enum class CommandType : u16 {
    FRAME = 0,
    CONTROLLER = 1,
    MOTION = 2,
    AMIIBO = 3,
    TOUCH = 4,

    // Editor Data
    COMMENT = 0x8000,

    INVALID = 0xffff
};

struct TouchEntry {
    u8 type = 0;
    u32 finger_id = 0;
    sead::Vector2i pos = sead::Vector2i::zero;
};

struct Command {
    CommandType type = CommandType::INVALID;
    u64 size = 0;
    u8* data;
};

struct CmdFrame {
    u32 frame = 0;
};

struct CmdController {
    u8 player = 0;
    u8 buttons[7] = {0, 0, 0, 0, 0, 0, 0};
    sead::Vector2i stickL = sead::Vector2i::zero;
    sead::Vector2i stickR = sead::Vector2i::zero;
};

struct CmdMotion {
    u8 player = 0;
    u8 conId = 2;
    u8 pad[2];
    sead::Vector3f accel = sead::Vector3f::zero;
    sead::Vector3f gyro = sead::Vector3f::zero;
};

struct CmdAmiibo {
    u64 amiibo = 0;  // model info
};

struct CmdTouch {
    u32 count = 0;
    TouchEntry entries[];
};

class Script {
public:
    Script(sead::Heap* heap) : mHeap(heap) {};
    ~Script() = default;

    hk::Result loadScript(const char* path);

    hk::ValueOrResult<Command*> tryReadCommand();

    u32 getFrames() { return mScriptHeader.frameCount; }

    int getPlayerCount() {
        int n = 0;
        for (int i = 0; i < 8; i++)
            if (mScriptHeader.conTypes[i] != 0)
                n++;
        return n;
    }

    bool is2P() { return getPlayerCount() == 2; }

private:
    template <typename T>
    T read() {
        T data = *(T*)&mData[mCursor];
        mCursor += sizeof(T);
        return data;
    }

    u64 mCursor = 0;
    sead::Heap* mHeap = nullptr;
    u8* mData = nullptr;
    FileHeader mFileHeader;
    ScriptHeader mScriptHeader;
    u64 mFileSize = 0;
};

class STAS : public al::NerveExecutor {
    SEAD_SINGLETON_DISPOSER(STAS);
    STAS();
    ~STAS();

public:
    bool tryLoadScript();
    void startScript();
    void endScript();
    void applyCommand(Command* cmd);
    void applyFrame();
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

    u32 getFrameCount() const { return mScript->getFrames(); };

    Script* getScript() { return mScript; };

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
    u32 mNextFrame = 0;
    u64 mPrevButtons[2] = {0, 0};

    Script* mScript = nullptr;
    al::Scene* mScene = nullptr;
    bool mIsUseAbsoluteJoystick = false;
    int mSpeed = 1;
    int mSpeedUntilFrame = 0;
    bool mHasSpeedUntilFrame = false;
};
