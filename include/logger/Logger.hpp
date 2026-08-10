#pragma once

#include "hk/Result.h"
#include "hk/prim/traits/Integer.h"

#include "sead/heap/seadHeap.h"

#include "devgui/savedata/DevGuiWriteStream.h"

#define LOGGERSAVEPATH "sd:/LunaKit/LKData/logger.byml"

enum class LoggerState { UNINITIALIZED = 0, CONNECTED = 1, UNAVAILABLE = 2, DISCONNECTED = 3 };

class Logger {
public:
    Logger() = default;

    static Logger& instance();

    hk::Result init(sead::Heap* heap);

    static void log(const char* fmt, ...);
    static void log(const char* fmt, va_list args);

    hk::Result writeLoggerSave(sead::Heap* heap, bool disable, const char* ip, u32 port);

    static bool getDisabledState() { return instance().mIsDisabled; }

private:
    LoggerState mState;
    int mSocketFd;
    bool mIsDisabled;

    sead::RamStreamSrc* mRamStream;
    DevGuiWriteStream* mWriteStream;
    u8 mWorkBuf[0x100];
};
