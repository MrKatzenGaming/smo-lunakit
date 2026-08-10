#include "helpers/fsHelper.h"

#include "hk/Result.h"

#include "nn/fs/fs_directories.h"
#include "nn/fs/fs_files.h"
#include "nn/fs/fs_types.h"

#include <cstdlib>

namespace FsHelper {
hk::Result writeFileToPath(void* buf, size_t size, const char* path) {
    nn::fs::FileHandle handle;

    if (isFileExist(path))
        nn::fs::DeleteFile(path);  // remove previous file

    hk::Result r = nn::fs::CreateFile(path, size).GetInnerValueForDebug();
    if (r.failed())
        return r;

    r = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Write).GetInnerValueForDebug();
    if (r.failed())
        return r;

    r = nn::fs::WriteFile(handle, 0, buf, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush)).GetInnerValueForDebug();
    if (r.failed())
        return r;

    nn::fs::CloseFile(handle);

    return hk::ResultSuccess();
}

hk::Result appendFileOnPath(void* buf, s64 pos, size_t size, const char* path) {
    nn::fs::FileHandle handle = {};
    hk::Result r;
    if (!isFileExist(path)) {
        r = nn::fs::CreateFile(path, size).GetInnerValueForDebug();
        if (r.failed())
            return r;
    }
    r = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Append).GetInnerValueForDebug();
    if (r.failed())
        return r;
    r = nn::fs::WriteFile(handle, pos, buf, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush)).GetInnerValueForDebug();
    if (r.failed())
        return r;
    nn::fs::CloseFile(handle);
    return hk::ResultSuccess();
}

// make sure to free buffer after usage is done
hk::Result loadFileFromPath(LoadData& loadData) {
    nn::fs::FileHandle handle;

    if (!FsHelper::isFileExist(loadData.path))
        return hk::ResultNotFound();

    if (hk::Result r = nn::fs::OpenFile(&handle, loadData.path, nn::fs::OpenMode_Read).GetInnerValueForDebug(); r.failed())
        return r;

    long size = 0;
    nn::fs::GetFileSize(&size, handle);
    loadData.bufSize = size;
    loadData.buffer = new u8[size];

    if (!loadData.buffer)
        return hk::ResultFailed();

    if (hk::Result r = nn::fs::ReadFile(handle, 0, loadData.buffer, size).GetInnerValueForDebug(); r.failed())
        return r;

    nn::fs::CloseFile(handle);
    return hk::ResultSuccess();
}

long getFileSize(const char* path) {
    nn::fs::FileHandle handle;
    long result = -1;

    hk::Result openResult = nn::fs::OpenFile(&handle, path, nn::fs::OpenMode::OpenMode_Read).GetInnerValueForDebug();

    if (openResult.succeeded()) {
        nn::fs::GetFileSize(&result, handle);
        nn::fs::CloseFile(handle);
    }

    return result;
}

bool isFileExist(const char* path) {
    nn::fs::DirectoryEntryType type;
    nn::fs::GetEntryType(&type, path);

    return type == nn::fs::DirectoryEntryType_File;
}

bool isDirExist(const char* path) {
    nn::fs::DirectoryEntryType type;
    nn::fs::GetEntryType(&type, path);

    return type == nn::fs::DirectoryEntryType_Directory;
}

}  // namespace FsHelper
