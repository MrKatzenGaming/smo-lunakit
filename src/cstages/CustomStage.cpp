#include "cstages/CustomStage.h"

#include "al/Library/File/FileUtil.h"
#include "al/Library/Yaml/ByamlUtil.h"

#include "heap/seadHeapMgr.h"
#include "helpers/fsHelper.h"

CustomStageEntry::CustomStageEntry(const char* stageName) {
    mStageName = stageName;

    sead::FormatFixedSafeString<0xff> stageMapPath("StageData/%sMap", stageName);
    mIsExist = al::isExistArchive(stageMapPath);
}

CustomStageCategory::CustomStageCategory(al::ByamlIter catIter, sead::Heap* heap) {
    sead::ScopedCurrentHeapSetter setter(heap);
    mIter = catIter;
    al::tryGetByamlString(&mCatName, catIter, "CategoryName");
    al::tryGetByamlString(&mCatDesc, catIter, "CategoryDesc");

    al::ByamlIter stageIter = catIter.getIterByKey("StageList");
    mCatSize = stageIter.getSize();

    mEntries.allocBuffer(mCatSize, heap);

    for (u32 i = 0; i < mCatSize; i++) {
        const char* stageName;
        stageIter.tryGetStringByIndex(&stageName, i);

        CustomStageEntry* newEntry = new CustomStageEntry(stageName);
        mEntries.pushBack(newEntry);
    }
}

CustomStageResource::CustomStageResource(const char* resourcePath, const char* resourceName, sead::Heap* heap) {
    sead::ScopedCurrentHeapSetter setter(heap);
    mResourceName = resourceName;

    FsHelper::LoadData loadData = {.path = resourcePath};

    FsHelper::loadFileFromPath(loadData);

    mRootByaml = al::ByamlIter(loadData.buffer);

    unsigned int size = mRootByaml.getSize();
    mCategories.allocBuffer(size, heap);

    for (unsigned int i = 0; i < size; i++) {
        al::ByamlIter catIter = mRootByaml.getIterByIndex(i);
        CustomStageCategory* newCat = new CustomStageCategory(catIter, heap);
        mCategories.pushBack(newCat);
    }
}
