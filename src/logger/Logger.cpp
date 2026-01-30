#include "logger/Logger.hpp"

#include "hk/Result.h"
#include "hk/diag/diag.h"
#include "hk/svc/api.h"

#include "nn/nifm.h"
#include "nn/socket.h"
#include "nn/util.h"
#include "vapours/results/results_common.hpp"

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

#include "heap/seadHeapMgr.h"

Logger& Logger::instance() {
    static Logger instance = {};
    return instance;
}

static constexpr int socketPoolSize = 0x600000;
static constexpr int socketAllocPoolSize = 0x20000;
char socketPool[socketPoolSize + socketAllocPoolSize] __attribute__((aligned(0x1000)));

s32 Logger::init(sead::Heap* heap) {
    if (mState != LoggerState::UNINITIALIZED)
        return -1;

    sead::ScopedCurrentHeapSetter setter(heap);

    sead::Stream::Modes streamMode = sead::Stream::Modes::Binary;
    mRamStream = new sead::RamStreamSrc(&mWorkBuf, sizeof(mWorkBuf));
    mWriteStream = new DevGuiWriteStream(mRamStream, streamMode);

    if (!FsHelper::isFileExist(LOGGERSAVEPATH)) {
        writeLoggerSave(heap, true, "0", 0);
        mIsDisabled = true;
        mState = LoggerState::CONNECTED;
        return 0;
    }

    FsHelper::LoadData loadData = {.path = LOGGERSAVEPATH};

    FsHelper::loadFileFromPath(loadData);
    al::ByamlIter root = al::ByamlIter((u8*)loadData.buffer);

    root.tryGetBoolByKey(&mIsDisabled, "Disable");

    if (mIsDisabled) {
        mState = LoggerState::CONNECTED;
        return 0;
    }

    in_addr hostAddress = {0};
    sockaddr_in serverAddress = {0};

    const char* ip = nullptr;
    u32 port;

    root.tryGetStringByKey(&ip, "IP");
    root.tryGetUIntByKey(&port, "Port");

    nn::Result initResult = nn::socket::Initialize(socketPool, socketPoolSize, socketAllocPoolSize, 0xE);

    HK_ABORT_UNLESS(initResult.IsSuccess(), "Failed Initializing Socket %d, %d, %s", nn::socket::GetLastErrno(), initResult.GetDescription(),
                    strerror(nn::socket::GetLastErrno()));

    nn::nifm::SubmitNetworkRequest();

    while (nn::nifm::IsNetworkRequestOnHold()) {
    }

    if (!nn::nifm::IsNetworkAvailable()) {
        mState = LoggerState::UNAVAILABLE;
        return -1;
    }

    if ((mSocketFd = nn::socket::Socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        mState = LoggerState::UNAVAILABLE;
        return -1;
    }

    nn::socket::InetAton(ip, &hostAddress);

    serverAddress.sin_addr = hostAddress;
    serverAddress.sin_port = nn::socket::InetHtons(port);
    serverAddress.sin_family = nn::socket::InetHtons(AF_INET);

    nn::Result result = nn::socket::Connect(mSocketFd, (sockaddr*)&serverAddress, sizeof(serverAddress));

    HK_ABORT_UNLESS(result.IsSuccess(), "Failed to connect logger %s", strerror(nn::socket::GetLastErrno()));

    mState = result.IsSuccess() ? LoggerState::CONNECTED : LoggerState::DISCONNECTED;

    if (mState == LoggerState::CONNECTED)
        Logger::log("Connected!\n");

    return result.GetInnerValueForDebug();
}

void Logger::log(const char* fmt, ...) {
    if (instance().mState != LoggerState::CONNECTED && !instance().mIsDisabled)
        return;

    va_list args;
    va_start(args, fmt);

    char buffer[0x500] = {};

    if (nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
        if (instance().mIsDisabled) {
            hk::svc::OutputDebugString(buffer, strlen(buffer));
        } else {
            char prefix[0x510];
            nn::util::SNPrintf(prefix, sizeof(prefix), "%s", buffer);
            nn::socket::Send(instance().mSocketFd, prefix, strlen(prefix), 0);
        }
    }

    va_end(args);
}

void Logger::log(const char* fmt, va_list args) {
    if (instance().mState != LoggerState::CONNECTED)
        return;

    char buffer[0x500] = {};

    if (nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
        char prefix[0x510];
        nn::util::SNPrintf(prefix, sizeof(prefix), "%s", buffer);
        nn::socket::Send(instance().mSocketFd, prefix, strlen(prefix), 0);
    }
}

s32 Logger::writeLoggerSave(sead::Heap* heap, bool disable, const char* ip, u32 port) {
    if (disable && !mIsDisabled)
        Logger::log("Logger disabled! Goodbye!\n");

    mWriteStream->rewind();
    al::ByamlWriter file = al::ByamlWriter(heap, false);

    file.pushHash();

    file.addBool("Disable", disable);
    file.addString("IP", ip);
    file.addUInt("Port", port);

    file.pop();
    file.write(mWriteStream);

    s32 result = FsHelper::writeFileToPath(mWorkBuf, file.calcPackSize(), LOGGERSAVEPATH).GetInnerValueForDebug();

    return result;
}
