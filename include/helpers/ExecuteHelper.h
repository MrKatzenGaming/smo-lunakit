#pragma once

#include "sead/container/seadObjArray.h"
#include "sead/container/seadStrTreeMap.h"

#include "al/Library/Execute/ExecuteOrder.h"

#include "heap/seadDisposer.h"

namespace al {
struct ExecuteSystemInitInfo;
class ExecuteTableHolderUpdate;
class ExecuteTableHolderDraw;
class ExecutorListBase;
}  // namespace al

class ExecuteHelper {
    SEAD_SINGLETON_DISPOSER(ExecuteHelper);

public:
    enum class DrawListType { Actor, ActorModel, Layout };
    enum class UpdateListType { Actor, Layout };

    using CreateDrawListFunction = al::ExecutorListBase* (*)(const al::ExecuteOrder* order, const al::ExecuteSystemInitInfo& initInfo);
    using CreateUpdateListFunction = al::ExecutorListBase* (*)(const al::ExecuteOrder* order);

    struct TableOverride {
        sead::FixedPtrArray<al::ExecuteOrder, 16> prepend;
        sead::FixedPtrArray<al::ExecuteOrder, 16> append;
    };

    struct CustomDrawList {
        DrawListType type;
        CreateDrawListFunction createFunc;
    };

    struct CustomUpdateList {
        UpdateListType type;
        CreateUpdateListFunction createFunc;
    };

    ExecuteHelper();
    ~ExecuteHelper();

    void prependDrawTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);
    void appendDrawTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);

    void prependUpdateTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);
    void appendUpdateTable(const sead::SafeString& tableName, const al::ExecuteOrder* order);

    void addCustomDrawList(const sead::SafeString& groupName, CreateDrawListFunction createFunc, DrawListType type);
    void addCustomUpdateList(const sead::SafeString& groupName, CreateUpdateListFunction createFunc, UpdateListType type);

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
