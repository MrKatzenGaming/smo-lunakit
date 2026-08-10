#include "cstages/CustomStageManager.h"

#include "hk/Result.h"

#include "nn/fs/fs_directories.h"

#include "sead/heap/seadHeapMgr.h"

#include "logger/Logger.hpp"

void CustomStageManager::init(sead::Heap* heap) {
    mHeap = heap;
    sead::ScopedCurrentHeapSetter setter(mHeap);
    Logger::log("Starting custom stage plugin loader from %s\n", CUSTOMSTAGEPATH);

    setupDirectoryInfo();

    mStageResources.tryAllocBuffer(mEntryCount, mHeap);
    for (int i = 0; i < mEntryCount; i++) {
        Logger::log("   Loading custom stage plugin %s\n", getFileName(i));

        sead::FormatFixedSafeString<0xff> filePath("%s%s", CUSTOMSTAGEPATH, getFileName(i));

        CustomStageResource* newRes = new CustomStageResource(filePath.cstr(), getFileName(i), mHeap);
        mStageResources.pushBack(newRes);
    }

    Logger::log("   Loaded all custom stages - Total %i\n", mEntryCount);
}

void CustomStageManager::setupDirectoryInfo() {
    sead::ScopedCurrentHeapSetter setter(mHeap);
    nn::fs::DirectoryHandle handle;
    hk::Result r = nn::fs::OpenDirectory(&handle, CUSTOMSTAGEPATH, nn::fs::OpenDirectoryMode_File).GetInnerValueForDebug();
    if (r.failed())
        return;
    s64 entryCount;
    r = nn::fs::GetDirectoryEntryCount(&entryCount, handle).GetInnerValueForDebug();
    if (r.failed()) {
        nn::fs::CloseDirectory(handle);
        return;
    }
    nn::fs::DirectoryEntry* entryBuffer = new (mHeap) nn::fs::DirectoryEntry[entryCount];
    r = nn::fs::ReadDirectory(&entryCount, entryBuffer, handle, entryCount).GetInnerValueForDebug();
    nn::fs::CloseDirectory(handle);
    if (r.failed()) {
        delete[] entryBuffer;
        return;
    }
    delete[] mEntries;
    mEntries = entryBuffer;
    mEntryCount = entryCount;
}
