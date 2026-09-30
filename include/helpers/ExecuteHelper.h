#pragma once

#include "sead/container/seadObjArray.h"
#include "sead/container/seadStrTreeMap.h"

#include "al/Library/Execute/ExecuteOrder.h"

#include "heap/seadDisposer.h"

namespace al {
class ExecuteTableHolderUpdate;
class ExecuteTableHolderDraw;
class ExecutorListBase;
}  // namespace al

class ExecuteHelper {
    SEAD_SINGLETON_DISPOSER(ExecuteHelper);

public:
    enum class DrawListType { Actor, ActorModel, Layout };
    enum class UpdateListType { Actor, Layout };

    using ListCreatorFunction = al::ExecutorListBase* (*)(const al::ExecuteOrder* order);

    struct TableOverride {
        sead::FixedPtrArray<al::ExecuteOrder, 16> prepend;
        sead::FixedPtrArray<al::ExecuteOrder, 16> append;
    };

    struct CustomDrawList {
        DrawListType type;
        ListCreatorFunction createFunc;
    };

    struct CustomUpdateList {
        UpdateListType type;
        ListCreatorFunction createFunc;
    };

    ExecuteHelper();
    ~ExecuteHelper();

    void prependDrawTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);
    void appendDrawTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);

    void prependUpdateTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);
    void appendUpdateTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);

    void addCustomDrawList(const sead::SafeString& groupName, ListCreatorFunction createFunc, DrawListType type);
    void addCustomUpdateList(const sead::SafeString& groupName, ListCreatorFunction createFunc, UpdateListType type);

    const TableOverride* findDrawTableOverride(const sead::SafeString& tableName) const;
    const TableOverride* findUpdateTableOverride(const sead::SafeString& tableName) const;

    const CustomDrawList* findCustomDrawListCreator(const sead::SafeString& groupName) const;
    const CustomUpdateList* findCustomUpdateListCreator(const sead::SafeString& groupName) const;

    bool hasCustomDrawList(const sead::SafeString& groupName, DrawListType type) const;
    bool hasCustomUpdateList(const sead::SafeString& groupName, UpdateListType type) const;

    s32 getCustomDrawListCount(DrawListType type) const;
    s32 getCustomUpdateListCount(UpdateListType type) const;

    static void installHooks();

private:
    sead::StrTreeMap<64, TableOverride*> mDrawTableOverrides;
    sead::StrTreeMap<64, TableOverride*> mUpdateTableOverrides;

    sead::StrTreeMap<64, CustomDrawList> mCustomDrawLists;
    sead::StrTreeMap<64, CustomUpdateList> mCustomUpdateLists;
};
