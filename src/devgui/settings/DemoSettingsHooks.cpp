#include "hk/hook/Trampoline.h"

#include "devgui/DevGuiManager.h"
#include "devgui/settings/DevGuiSettings.h"

#define HOOK_CALLBACK(NAME)                                                                                                                          \
    HkTrampoline NAME = [](TrampolineStatic(), void* thisPtr) -> bool {                                                                              \
        DevGuiSettings* set = DevGuiManager::instance()->getSettings();                                                                              \
        if (set->getStateByName("Always Manually Skip Cutscenes"))                                                                                   \
            return true;                                                                                                                             \
        return orig(thisPtr);                                                                                                                        \
    };

HOOK_CALLBACK(RsDemoHook)
HOOK_CALLBACK(FirstDemoScenarioHook)
HOOK_CALLBACK(FirstDemoWorldHook)
HOOK_CALLBACK(FirstDemoMoonRockHook)
HOOK_CALLBACK(ShowDemoHackHook)

void exlSetupDemoHooks() {
    RsDemoHook.installAtSym<"_ZN2rs11isFirstDemoEPKN2al5SceneE">();
    FirstDemoScenarioHook.installAtSym<"_ZN2rs30isFirstDemoScenarioStartCameraEPKN2al9LiveActorE">();
    FirstDemoWorldHook.installAtSym<"_ZN2rs27isFirstDemoWorldIntroCameraEPKN2al5SceneE">();
    FirstDemoMoonRockHook.installAtSym<"_ZNK12MoonRockData38isEnableShowDemoAfterOpenMoonRockFirstEv">();
    ShowDemoHackHook.installAtSym<"_ZNK18DemoStateHackFirst20isEnableShowHackDemoEv">();
}
