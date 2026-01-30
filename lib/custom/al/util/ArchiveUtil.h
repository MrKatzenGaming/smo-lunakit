#pragma once

#include <sead/prim/seadSafeString.h>
#include <sead/resource/seadArchiveRes.h>
#include <sead/resource/seadResource.h>

namespace al {
sead::ArchiveRes* loadArchive(const sead::SafeString& arcPath);
sead::ArchiveRes* loadArchiveWithExt(const sead::SafeString& arcPath, const char* ext);
};  // namespace al
