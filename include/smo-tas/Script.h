#pragma once

#include "hk/Result.h"
#include "hk/ValueOrResult.h"

#include "sead/heap/seadHeap.h"
#include "sead/math/seadQuat.h"
#include "sead/math/seadVectorFwd.h"

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
};

enum class CommandType : u16 {
    FRAME = 0,
    CONTROLLER = 1,
    MOTION = 2,
    AMIIBO = 3,
    TOUCH = 4,

    // Editor Data
    COMMENT = 0x8000,

    // Game specific
    SAVEFILE = 0xc000,
    GO = 0xc001,
    TPMARIO = 0xc002,
    TPCAP = 0xc003,
    ABSSTICK = 0xc004,
    SPEEDUP = 0xc005,
    PAUSE = 0xc006,
    DEMO = 0xc007,

    INVALID = 0xffff
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
    struct TouchEntry {
        u8 type = 0;
        u32 finger_id = 0;
        sead::Vector2i pos = sead::Vector2i::zero;
    };

    u32 count = 0;
    TouchEntry entries[];
};

struct CmdSaveFile {
    s8 fileId = 0;
    bool reload = false;
};

struct CmdGo {
    s8 scenario = 0;
    s8 subScenario = 0;
    bool returnPrev = false;
    char pad[1];
    u16 stageLen;
    char* stageName;
    u16 entrLen;
    char* entrId;
};

struct CmdTpMarCap {
    sead::Vector3f pos = sead::Vector3f::zero;
    sead::Quatf rot = sead::Quatf::unit;
};

struct CmdAbsStick {
    bool enable = false;
};

struct CmdSpeedup {
    u8 speed = 1;
};

struct CmdDemo {
    bool enable = false;
};

class Script {
private:
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
        char* author = nullptr;
        u32 title_len = 0;
        char* title = nullptr;
        u32 desc_len = 0;
        char* desc = nullptr;
    };

#pragma pack(pop)

public:
    Script(sead::Heap* heap) : mHeap(heap) {};

    ~Script() { unload(); };

    hk::Result load(const char* path);
    void unload();

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
