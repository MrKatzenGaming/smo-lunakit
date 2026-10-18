#include "helpers/ExecuteHelper.h"

#include "hk/hook/Trampoline.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/Execute/ExecuteTablesImpl.h"

#include "logger/Logger.hpp"

SEAD_SINGLETON_DISPOSER_IMPL(ExecuteHelper);

ExecuteHelper::ExecuteHelper() {
    mDrawTableOverrides.allocBuffer(DrawTableSize, nullptr);
    mUpdateTableOverrides.allocBuffer(UpdateTableSize, nullptr);

    mCustomDrawLists.allocBuffer(32, nullptr);
    mCustomUpdateLists.allocBuffer(32, nullptr);

    installHooks();
}

ExecuteHelper::~ExecuteHelper() {
    mDrawTableOverrides.freeBuffer();
    mUpdateTableOverrides.freeBuffer();

    mCustomDrawLists.freeBuffer();
    mCustomUpdateLists.freeBuffer();
}

void ExecuteHelper::prependDrawTable(const sead::SafeString& tableName, const al::ExecuteOrder* order) {
    if (auto* node = mDrawTableOverrides.find(tableName)) {
        node->value()->prepend.pushBack(const_cast<al::ExecuteOrder*>(order));
    } else {
        auto* override = *mDrawTableOverrides.insert(tableName, new TableOverride());
        override->prepend.pushBack(const_cast<al::ExecuteOrder*>(order));
    }
}

void ExecuteHelper::appendDrawTable(const sead::SafeString& tableName, const al::ExecuteOrder* order) {
    if (auto* node = mDrawTableOverrides.find(tableName)) {
        node->value()->append.pushBack(const_cast<al::ExecuteOrder*>(order));
    } else {
        auto* override = *mDrawTableOverrides.insert(tableName, new TableOverride());
        override->append.pushBack(const_cast<al::ExecuteOrder*>(order));
    }
}

void ExecuteHelper::prependUpdateTable(const sead::SafeString& tableName, const al::ExecuteOrder* order) {
    if (auto* node = mUpdateTableOverrides.find(tableName)) {
        node->value()->prepend.pushBack(const_cast<al::ExecuteOrder*>(order));
    } else {
        auto* override = *mUpdateTableOverrides.insert(tableName, new TableOverride());
        override->prepend.pushBack(const_cast<al::ExecuteOrder*>(order));
    }
}

void ExecuteHelper::appendUpdateTable(const sead::SafeString& tableName, const al::ExecuteOrder* order) {
    if (auto* node = mUpdateTableOverrides.find(tableName)) {
        node->value()->append.pushBack(const_cast<al::ExecuteOrder*>(order));
    } else {
        auto* override = *mUpdateTableOverrides.insert(tableName, new TableOverride());
        override->append.pushBack(const_cast<al::ExecuteOrder*>(order));
    }
}

void ExecuteHelper::addCustomDrawList(const sead::SafeString& groupName, const CreateDrawListFunction createFunc, const DrawListType type) {
    mCustomDrawLists.insert(groupName, CustomDrawList{.type = type, .createFunc = createFunc});
}

void ExecuteHelper::addCustomUpdateList(const sead::SafeString& groupName, const CreateUpdateListFunction createFunc, const UpdateListType type) {
    mCustomUpdateLists.insert(groupName, CustomUpdateList{.type = type, .createFunc = createFunc});
}

const ExecuteHelper::TableOverride* ExecuteHelper::findDrawTableOverride(const sead::SafeString& tableName) const {
    if (auto* node = mDrawTableOverrides.find(tableName))
        return node->value();

    return nullptr;
}

const ExecuteHelper::TableOverride* ExecuteHelper::findUpdateTableOverride(const sead::SafeString& tableName) const {
    if (auto* node = mUpdateTableOverrides.find(tableName))
        return node->value();

    return nullptr;
}

const ExecuteHelper::CustomDrawList* ExecuteHelper::findCustomDrawListCreator(const sead::SafeString& groupName) const {
    if (auto* node = mCustomDrawLists.find(groupName))
        return &node->value();

    return nullptr;
}

const ExecuteHelper::CustomUpdateList* ExecuteHelper::findCustomUpdateListCreator(const sead::SafeString& groupName) const {
    if (auto* node = mCustomUpdateLists.find(groupName))
        return &node->value();

    return nullptr;
}

bool ExecuteHelper::hasCustomDrawList(const sead::SafeString& groupName, const DrawListType type) const {
    if (const auto* customList = mCustomDrawLists.find(groupName))
        return customList->value().type == type;

    return false;
}

bool ExecuteHelper::hasCustomUpdateList(const sead::SafeString& groupName, const UpdateListType type) const {
    if (const auto* customList = mCustomUpdateLists.find(groupName))
        return customList->value().type == type;

    return false;
}

s32 ExecuteHelper::getCustomDrawListCount(DrawListType type) const {
    s32 count = 0;
    mCustomDrawLists.forEach([&count, type](const sead::SafeString&, const CustomDrawList& list) {
        if (list.type == type)
            count++;
    });
    return count;
}

s32 ExecuteHelper::getCustomUpdateListCount(UpdateListType type) const {
    s32 count = 0;
    mCustomUpdateLists.forEach([&count, type](const sead::SafeString&, const CustomUpdateList& list) {
        if (list.type == type)
            count++;
    });
    return count;
}

HkTrampoline hookDrawTableInit = [](TrampolineStatic(), al::ExecuteTableHolderDraw* self, const char* name, const al::ExecuteSystemInitInfo& initInfo,
                                    const al::ExecuteOrder* orders, s32 orderCount) -> void {
    if (auto* override = SEAD_INSTANCE(ExecuteHelper)->findDrawTableOverride(name)) {
        s32 newOrderCount = orderCount + override->prepend.size() + override->append.size();
        auto* newOrders = new al::ExecuteOrder[newOrderCount];

        s32 idx = 0;
        for (const auto order : override->prepend)
            newOrders[idx++] = order;
        for (s32 i = 0; i < orderCount; i++)
            newOrders[idx++] = orders[i];
        for (const auto order : override->append)
            newOrders[idx++] = order;

        orig(self, name, initInfo, newOrders, newOrderCount);
    } else {
        orig(self, name, initInfo, orders, orderCount);
    }
};

HkTrampoline hookUpdateTableInit = [](TrampolineStatic(), al::ExecuteTableHolderUpdate* self, const char* name,
                                      const al::ExecuteSystemInitInfo& initInfo, const al::ExecuteOrder* orders, s32 orderCount) -> void {
    if (auto* override = SEAD_INSTANCE(ExecuteHelper)->findUpdateTableOverride(name)) {
        s32 newOrderCount = orderCount + override->prepend.size() + override->append.size();
        auto* newOrders = new al::ExecuteOrder[newOrderCount];

        s32 idx = 0;
        for (const auto order : override->prepend)
            newOrders[idx++] = order;
        for (s32 i = 0; i < orderCount; i++)
            newOrders[idx++] = orders[i];
        for (const auto order : override->append)
            newOrders[idx++] = order;

        orig(self, name, initInfo, newOrders, newOrderCount);
    } else {
        orig(self, name, initInfo, orders, orderCount);
    }
};

static al::ExecutorListBase* registerCustomDrawList(al::ExecuteTableHolderDraw* table, const al::ExecuteOrder* order,
                                                    const al::ExecuteSystemInitInfo& initInfo) {
    if (const auto customList = SEAD_INSTANCE(ExecuteHelper)->findCustomDrawListCreator(order->executeGroup)) {
        auto* list = customList->createFunc(order, initInfo);

        switch (customList->type) {
        case ExecuteHelper::DrawListType::Actor:
            table->registerExecutorListActor(reinterpret_cast<al::ExecutorListActorDraw*>(list));
            Logger::log("Created custom draw list '%s' for type Actor on table '%s'", order->executeGroup, table->getName());
            break;
        case ExecuteHelper::DrawListType::ActorModel:
            table->registerExecutorListActorModel(reinterpret_cast<al::ExecutorListActorModelDrawBase*>(list));
            Logger::log("Created custom draw list '%s' for type ActorModel on table '%s'", order->executeGroup, table->getName());
            break;
        case ExecuteHelper::DrawListType::Layout:
            table->registerExecutorListLayout(reinterpret_cast<al::ExecutorListLayoutDrawBase*>(list));
            Logger::log("Created custom draw list '%s' for type Layout on table '%s'", order->executeGroup, table->getName());
            break;
        }

        return list;
    }

    return nullptr;
}

static al::ExecutorListBase* registerCustomUpdateList(al::ExecuteTableHolderUpdate* table, const al::ExecuteOrder* order) {
    if (const auto customList = SEAD_INSTANCE(ExecuteHelper)->findCustomUpdateListCreator(order->executeGroup)) {
        auto* list = customList->createFunc(order);

        switch (customList->type) {
        case ExecuteHelper::UpdateListType::Actor:
            table->registerExecutorListActor(reinterpret_cast<al::ExecutorListActorExecuteBase*>(list));
            Logger::log("Created custom update list '%s' for type Actor on table '%s' ", order->executeGroup, table->getName());
            break;
        case ExecuteHelper::UpdateListType::Layout:
            table->registerExecutorListLayout(reinterpret_cast<al::ExecutorListLayoutUpdate*>(list));
            Logger::log("Created custom update list '%s' for type Layout on table '%s'", order->executeGroup, table->getName());
            break;
        }

        return list;
    }

    return nullptr;
}

#define PROLOG_SAVE_X0_X1                                                                                                                            \
    "stp x29, x30, [sp, #-32]!\n"                                                                                                                    \
    "mov x29, sp\n"                                                                                                                                  \
    "stp x0, x1, [sp, #16]\n"

#define EPILOG_RESTORE_X0_X1                                                                                                                         \
    "ldp x0, x1, [sp, #16]\n"                                                                                                                        \
    "ldp x29, x30, [sp], #32\n"                                                                                                                      \
    "ret\n"

#define PROLOG_SAVE_X0_X1_X2_X3                                                                                                                      \
    "stp x29, x30, [sp, #-64]!\n"                                                                                                                    \
    "mov x29, sp\n"                                                                                                                                  \
    "stp x0, x1, [sp, #16]\n"                                                                                                                        \
    "stp x2, x3, [sp, #32]\n"

#define EPILOG_RESTORE_X0_X1_X2_X3                                                                                                                   \
    "ldp x0, x1, [sp, #16]\n"                                                                                                                        \
    "ldp x2, x3, [sp, #32]\n"                                                                                                                        \
    "ldp x29, x30, [sp], #64\n"                                                                                                                      \
    "ret\n"

static void registerCustomDrawListWrapper() {
    asm volatile(PROLOG_SAVE_X0_X1_X2_X3);
    asm volatile("mov x0, x21\n"
                 "mov x1, x19\n"
                 "mov x2, x20\n"
                 "bl %0\n"
                 "mov x27, x0\n"
                 :
                 : "X"(registerCustomDrawList));
    asm volatile(EPILOG_RESTORE_X0_X1_X2_X3);
}

static void registerCustomUpdateListWrapper() {
    asm volatile(PROLOG_SAVE_X0_X1);
    asm volatile("mov x0, x20\n"
                 "mov x1, x19\n"
                 "bl %0\n"
                 "mov x28, x0\n"
                 :
                 : "X"(registerCustomUpdateList));
    asm volatile(EPILOG_RESTORE_X0_X1);
}

HkTrampoline hookCalcDrawListActorNumMax = [](TrampolineStatic(), const al::ExecuteOrder* orders, s32 orderNum) -> s32 {
    s32 count = orig(orders, orderNum);
    for (s32 i = 0; i < orderNum; i++)
        if (SEAD_INSTANCE(ExecuteHelper)->hasCustomDrawList(orders[i].executeGroup, ExecuteHelper::DrawListType::Actor))
            count++;
    return count;
};
HkTrampoline hookCalcDrawListActorModelNumMax = [](TrampolineStatic(), const al::ExecuteOrder* orders, s32 orderNum) -> s32 {
    s32 count = orig(orders, orderNum);
    for (s32 i = 0; i < orderNum; i++)
        if (SEAD_INSTANCE(ExecuteHelper)->hasCustomDrawList(orders[i].executeGroup, ExecuteHelper::DrawListType::ActorModel))
            count++;
    return count;
};
HkTrampoline hookCalcDrawListLayoutNumMax = [](TrampolineStatic(), const al::ExecuteOrder* orders, s32 orderNum) -> s32 {
    s32 count = orig(orders, orderNum);
    for (s32 i = 0; i < orderNum; i++)
        if (SEAD_INSTANCE(ExecuteHelper)->hasCustomDrawList(orders[i].executeGroup, ExecuteHelper::DrawListType::Layout))
            count++;
    return count;
};

HkTrampoline hookCalcUpdateListActorNumMax = [](TrampolineStatic(), const al::ExecuteOrder* orders, s32 orderNum) -> s32 {
    s32 count = orig(orders, orderNum);
    for (s32 i = 0; i < orderNum; i++)
        if (SEAD_INSTANCE(ExecuteHelper)->hasCustomUpdateList(orders[i].executeGroup, ExecuteHelper::UpdateListType::Actor))
            count++;
    return count;
};
HkTrampoline hookCalcUpdateListLayoutNumMax = [](TrampolineStatic(), const al::ExecuteOrder* orders, s32 orderNum, const char* listName) -> s32 {
    s32 count = orig(orders, orderNum, listName);
    if (al::isEqualString(listName, "LayoutUpdate")) {
        for (s32 i = 0; i < orderNum; i++)
            if (SEAD_INSTANCE(ExecuteHelper)->hasCustomUpdateList(orders[i].executeGroup, ExecuteHelper::UpdateListType::Layout))
                count++;
    }
    return count;
};

void ExecuteHelper::installHooks() {
    hookDrawTableInit.installAtSym<"_ZN2al22ExecuteTableHolderDraw4initEPKcRKNS_21ExecuteSystemInitInfoEPKNS_12ExecuteOrderEi">();
    hookUpdateTableInit.installAtSym<"_ZN2al22ExecuteTableHolderDraw4initEPKcRKNS_21ExecuteSystemInitInfoEPKNS_12ExecuteOrderEi">();

    hookCalcDrawListActorNumMax.installAtSym<"_ZN18alExecutorFunction23calcDrawListActorNumMaxEPKN2al12ExecuteOrderEi">();
    hookCalcDrawListActorModelNumMax.installAtSym<"_ZN18alExecutorFunction28calcDrawListActorModelNumMaxEPKN2al12ExecuteOrderEi">();
    hookCalcDrawListLayoutNumMax.installAtSym<"_ZN18alExecutorFunction24calcDrawListLayoutNumMaxEPKN2al12ExecuteOrderEi">();

    hookCalcUpdateListActorNumMax.installAtSym<"_ZN18alExecutorFunction25calcUpdateListActorNumMaxEPKN2al12ExecuteOrderEi">();
    hookCalcUpdateListLayoutNumMax.installAtSym<"_ZN18alExecutorFunction22calcExecutorListNumMaxEPKN2al12ExecuteOrderEiPKc">();

    hk::hook::writeBranchLinkAtSym<"$registerDrawListHook">(registerCustomDrawListWrapper);
    hk::hook::writeBranchLinkAtSym<"$registerUpdateListHook">(registerCustomUpdateListWrapper);
}
