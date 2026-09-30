#include "custom/al/Library/Execute/ActorExecuteInfo.h"

#include "hk/hook/Replace.h"
#include "hk/hook/a64/Assembler.h"

#include "sead/math/seadMathCalcCommon.h"
#include "sead/prim/seadMemUtil.h"

namespace al {

void CustomActorExecuteInfo::ctor(ExecuteRequestKeeper* keeper) {
    mRequestKeeper = keeper;

    mUpdaterCount = 0;
    mCustomUpdaterCapacity = 0;
    mCustomUpdaters = nullptr;

    mDrawerCount = 0;
    mCustomDrawerCapacity = 0;
    mCustomDrawers = nullptr;
}

void CustomActorExecuteInfo::addUpdater(ExecutorActorExecuteBase* updater) {
    if (mUpdaterCount >= mCustomUpdaterCapacity) {
        mCustomUpdaterCapacity = sead::Mathi::max(4, mCustomUpdaterCapacity * 2);
        auto* newUpdaters = new ExecutorActorExecuteBase*[mCustomUpdaterCapacity];
        sead::MemUtil::copy(newUpdaters, mCustomUpdaters, mUpdaterCount * sizeof(ExecutorActorExecuteBase*));
        delete[] mCustomUpdaters;
        mCustomUpdaters = newUpdaters;
    }

    mCustomUpdaters[mUpdaterCount] = updater;
    mUpdaterCount++;
}

void CustomActorExecuteInfo::addDrawer(ModelDrawerBase* drawer) {
    if (mDrawerCount >= mCustomDrawerCapacity) {
        mCustomDrawerCapacity = sead::Mathi::max(11, mCustomDrawerCapacity * 2);
        auto* newDrawers = new ModelDrawerBase*[mCustomDrawerCapacity];
        sead::MemUtil::copy(newDrawers, mCustomDrawers, mDrawerCount * sizeof(ModelDrawerBase*));
        delete[] mCustomDrawers;
        mCustomDrawers = newDrawers;
    }

    mCustomDrawers[mDrawerCount] = drawer;
    mDrawerCount++;
}

HkReplace<void, ActorExecuteInfo*, ExecuteRequestKeeper*> hookCtor = [](ActorExecuteInfo* self, ExecuteRequestKeeper* keeper) -> void {
    auto* custom = reinterpret_cast<CustomActorExecuteInfo*>(self);
    custom->ctor(keeper);
};
HkReplace<void, ActorExecuteInfo*, ExecutorActorExecuteBase*> hookAddUpdater = [](ActorExecuteInfo* self, ExecutorActorExecuteBase* updater) -> void {
    auto* custom = reinterpret_cast<CustomActorExecuteInfo*>(self);
    custom->addUpdater(updater);
};
HkReplace<void, ActorExecuteInfo*, ModelDrawerBase*> hookAddDrawer = [](ActorExecuteInfo* self, ModelDrawerBase* drawer) -> void {
    auto* custom = reinterpret_cast<CustomActorExecuteInfo*>(self);
    custom->addDrawer(drawer);
};

template <hk::util::TemplateString Symbol, u32 InstrNum>
static hk::Result installAsmAtSym(const hk::hook::Instr (*instrs)[InstrNum]) {
    const ptr addr = hk::util::lookupSymbol<Symbol>();
    const auto* module = hk::ro::getModuleContaining(addr);
    HK_TRY(module->writeRo(addr - module->range().start(), instrs, sizeof(*instrs)));
    return hk::ResultSuccess();
}

void CustomActorExecuteInfo::installHooks() {
    hookCtor.installAtSym<"_ZN2al16ActorExecuteInfoC1EPNS_20ExecuteRequestKeeperE">();
    hookCtor.installAtSym<"_ZN2al16ActorExecuteInfoC2EPNS_20ExecuteRequestKeeperE">();
    hookAddUpdater.installAtSym<"_ZN2al16ActorExecuteInfo10addUpdaterEPNS_24ExecutorActorExecuteBaseE">();
    hookAddDrawer.installAtSym<"_ZN2al16ActorExecuteInfo9addDrawerEPNS_15ModelDrawerBaseE">();

    // info->mCustomDrawers
    // ldr x24, [x22, #0x10]
    constexpr hk::hook::Instr hook1[] = {0xF9400AD8};
    installAsmAtSym<"$actorExecuteInfoUpdaters1">(&hook1);
    installAsmAtSym<"$actorExecuteInfoUpdaters2">(&hook1);

    // info->mCustomDrawers
    // ldr x24, [x22, #0x38]
    constexpr hk::hook::Instr hook2[] = {0xF9401ED8};
    installAsmAtSym<"$actorExecuteInfoDrawers1">(&hook2);
    installAsmAtSym<"$actorExecuteInfoDrawers2">(&hook2);

    // info->mCustomDrawers[i]
    // ldr x8, [x8, #0x38]
    // ldr x0, [x8, x21, LSL #0x3]
    constexpr hk::hook::Instr hook3[] = {0xF9401D08, 0xF8757900};
    installAsmAtSym<"$actorExecuteInfoDrawers3">(&hook3);
}

}  // namespace al
