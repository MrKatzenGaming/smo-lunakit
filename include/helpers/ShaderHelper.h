#pragma once

#include "sead/container/seadPtrArray.h"
#include "sead/heap/seadDisposer.h"
#include "sead/prim/seadSafeString.h"

namespace agl {
class ShaderProgram;
class ShaderProgramArchive;
}  // namespace agl

namespace al {
class Resource;
}

class ShaderHelper {
    SEAD_SINGLETON_DISPOSER(ShaderHelper);

public:
    ShaderHelper();
    ~ShaderHelper();

    const agl::ShaderProgram* getShaderProgram(const sead::SafeString& name) const;

private:
    void loadResource(const al::Resource* res);

private:
    sead::FixedPtrArray<agl::ShaderProgramArchive, 16> mArchives;
};
