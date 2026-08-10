#pragma once

#include "hk/Result.h"

namespace FsHelper {

struct LoadData {
    const char* path;
    u8* buffer;
    long bufSize;
};

hk::Result writeFileToPath(void* buf, size_t size, const char* path);

hk::Result appendFileOnPath(void* buf, s64 pos, size_t size, const char* path);

hk::Result loadFileFromPath(LoadData& loadData);

long getFileSize(const char* path);

bool isFileExist(const char* path);
bool isDirExist(const char* path);
}  // namespace FsHelper
