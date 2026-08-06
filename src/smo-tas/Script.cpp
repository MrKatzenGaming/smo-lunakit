#include "smo-tas/Script.h"

#include "hk/Result.h"
#include "hk/diag/diag.h"
#include "hk/prim/traits/Integer.h"

#include "sead/heap/seadHeapMgr.h"

#include "devgui/DevGuiManager.h"
#include "logger/Logger.hpp"

hk::Result Script::load(const char* path) {
    sead::ScopedCurrentHeapSetter s(mHeap);
    FsHelper::LoadData data{.path = path};
    if (FsHelper::loadFileFromPath(data).failed())
        return hk::ResultFailed();

    mData = (u8*)data.buffer;
    mCursor = 0;
    mFileSize = data.bufSize;

    if (mFileSize < sizeof(FileHeader)) {
        unload();
        Logger::log("File too small\n");
        return hk::ResultInvalidSize();
    }

    mFileHeader = *(FileHeader*)&mData[mCursor];
    mCursor += sizeof(mFileHeader);

    if (mFileHeader.magic[0] != 'S' || mFileHeader.magic[1] != 'T' || mFileHeader.magic[2] != 'A' || mFileHeader.magic[3] != 'S') {
        unload();
        Logger::log("Wrong Header\n");
        return hk::ResultFailed();
    }

    if (mFileHeader.title_id != 0x0100000000010000) {
        unload();
        Logger::log("Wrong Title ID\n");
        return hk::ResultFailed();
    }

    switch (mFileHeader.version) {
    case 1: {
        if (mCursor + 20 > mFileSize) {
            unload();
            Logger::log("Script Header too small\n");
            return hk::ResultInvalidSize();
        }
        mScriptHeader.cmdCount = read<u32>();
        mScriptHeader.frameCount = read<u32>();
        mScriptHeader.editingSeconds = read<u32>();

        for (int i = 0; i < 8; i++)
            mScriptHeader.conTypes[i] = read<u8>();

        {
            mScriptHeader.author_len = read<u16>();
            if (mCursor + mScriptHeader.author_len > mFileSize) {
                unload();
                return hk::ResultInvalidSize();
            }
            if (mScriptHeader.author_len)
                mScriptHeader.author = (char*)&mData[mCursor];

            mCursor += mScriptHeader.author_len;
            mCursor = hk::alignUp(mCursor, 4);

            // hk::diag::logLine("Author: %s", mScriptHeader.author);
        }

        {
            mScriptHeader.title_len = read<u32>();
            if (mCursor + mScriptHeader.title_len > mFileSize) {
                unload();
                return hk::ResultInvalidSize();
            }
            if (mScriptHeader.title_len)
                mScriptHeader.title = (char*)&mData[mCursor];

            mCursor += mScriptHeader.title_len;
            mCursor = hk::alignUp(mCursor, 4);
            // hk::diag::logLine("Title: %s", mScriptHeader.title);
        }

        {
            mScriptHeader.desc_len = read<u32>();
            if (mCursor + mScriptHeader.desc_len > mFileSize) {
                unload();
                return hk::ResultInvalidSize();
            }
            if (mScriptHeader.desc_len)
                mScriptHeader.desc = (char*)&mData[mCursor];

            mCursor += mScriptHeader.desc_len;
            mCursor = hk::alignUp(mCursor, 4);
            // hk::diag::logLine("Description: %s", mScriptHeader.desc);
        }

        return hk::ResultSuccess();
    }
    default:
        Logger::log("Unsupported STAS version\n");
        unload();
        return hk::ResultNotImplemented();
    }
}

void Script::unload() {
    free(mData);
    mData = nullptr;
    mCursor = 0;
    mFileSize = 0;
    mFileHeader = FileHeader();
    mScriptHeader = ScriptHeader();
}

hk::ValueOrResult<Command*> Script::tryReadCommand() {
    sead::ScopedCurrentHeapSetter s(DevGuiManager::instance()->getHeap());

    if (mCursor >= mFileSize)
        return hk::ResultOutOfRange();

    CommandType type = read<CommandType>();

    u64 size = read<u64>() & 0xffffffffffff;
    mCursor -= 2;

    if (size == 0 && type != CommandType::PAUSE)
        return hk::ResultOutOfRange();

    Command* c = new Command{.type = type, .size = size, .data = &mData[mCursor]};
    mCursor += size;

    return hk::ValueOrResult(c);
}
