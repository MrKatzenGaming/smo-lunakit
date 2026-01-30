#pragma once

#include "Library/Yaml/ByamlIter.h"

#include <filedevice/seadFileDevice.h>
#include <prim/seadSafeString.hpp>

namespace al {
struct IAudioResourceLoader;
}

namespace al {

class FileLoader {
public:
    FileLoader(int);
    bool isExistFile(const sead::SafeString&, sead::FileDevice*) const;
    bool isExistArchive(const sead::SafeString&, sead::FileDevice*) const;
    bool isExistDirectory(const sead::SafeString&, sead::FileDevice*) const;

    void getFileDevice(const sead::SafeString&, sead::FileDevice*) const;
    void getFileSize(const sead::SafeString&, sead::FileDevice*) const;

    u32 listFiles(sead::FixedSafeString<256>*, int, const char*, const char*);
    u32 listSubdirectories(sead::FixedSafeString<256>*, int, const char*);

    void loadFile(const sead::SafeString&, int, sead::FileDevice*);
    void tryLoadFileToBuffer(const sead::SafeString&, unsigned char*, unsigned int, int, sead::FileDevice*);
    void loadArchive(const sead::SafeString&, sead::FileDevice*);
    void loadArchiveLocal(const sead::SafeString&, const char*, sead::FileDevice*);
    void loadArchiveWithExt(const sead::SafeString&, const char*, sead::FileDevice*);
    void tryRequestLoadArchive(const sead::SafeString&, sead::Heap*, sead::FileDevice*);
    void requestLoadArchive(const sead::SafeString&, sead::Heap*, sead::FileDevice*);
    void loadSoundItem(unsigned int, unsigned int, al::IAudioResourceLoader*);
    void requestLoadSoundItem(unsigned int, unsigned int, al::IAudioResourceLoader*);
    void tryRequestLoadSoundItem(unsigned int, al::IAudioResourceLoader*);
    void requestPreLoadFile(const al::ByamlIter&, sead::Heap*, al::IAudioResourceLoader*);
    void waitLoadDoneAllFile();
    void clearAllEntry();
    void setThreadPriority(int);
};

}  // namespace al
