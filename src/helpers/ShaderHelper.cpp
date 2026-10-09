#include "helpers/ShaderHelper.h"

#include "sead/gfx/seadGraphics.h"
#include "sead/heap/seadDisposer.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/Resource/Resource.h"

#include "agl/common/aglShaderProgram.h"
#include "agl/common/aglShaderProgramArchive.h"

#include "logger/Logger.hpp"

SEAD_SINGLETON_DISPOSER_IMPL(ShaderHelper)

static constexpr const char* shaderResources[] = {"ShaderData/lkRenderCollision"};

ShaderHelper::ShaderHelper() {
    for (const char* resource : shaderResources)
        loadResource(new al::Resource(resource));
}

ShaderHelper::~ShaderHelper() = default;

const agl::ShaderProgram* ShaderHelper::getShaderProgram(const sead::SafeString& name) const {
    for (const auto& archive : mArchives)
        if (const s32 index = archive.searchShaderProgramIndex(name); index >= 0)
            return archive.mShaderPrograms.get(index);

    return nullptr;
}

void ShaderHelper::loadResource(const al::Resource* res) {
    for (s32 i = 0; i < res->getEntryNum("/"); i++) {
        sead::FixedSafeString<256> entryName;
        res->getEntryName(&entryName, "/", i);

        if (al::isEqualSubString(entryName, ".sharcb")) {
            sead::Graphics::instance()->lockDrawContext();

            auto* archive = new agl::ShaderProgramArchive();
            archive->createWithOption(agl::ResBinaryShaderArchive(res->getOtherFile(entryName)), agl::ResShaderArchive(), 0, nullptr);
            archive->setUp();
            mArchives.pushBack(archive);

            sead::Graphics::instance()->unlockDrawContext();

            Logger::log("Loaded custom shader archive '%s'", entryName.cstr());
        }
    }
}
