#include "hk/hook/Trampoline.h"

#include "devgui/DevGuiManager.h"
#include "devgui/settings/DevGuiSettings.h"
#include "devgui/settings/MassHookDefine.h"

MASS_HOOK_DEFINER(RsDemoHook, FirstDemoScenarioHook, FirstDemoWorldHook, FirstDemoMoonRockHook, ShowDemoHackHook)

void exlSetupDemoHooks() {
    RsDemoHook.installAtSym<"_ZN2rs11isFirstDemoEPKN2al5SceneE">();
    FirstDemoScenarioHook.installAtSym<"_ZN2rs30isFirstDemoScenarioStartCameraEPKN2al9LiveActorE">();
    FirstDemoWorldHook.installAtSym<"_ZN2rs27isFirstDemoWorldIntroCameraEPKN2al5SceneE">();
    FirstDemoMoonRockHook.installAtSym<"_ZNK12MoonRockData38isEnableShowDemoAfterOpenMoonRockFirstEv">();
    ShowDemoHackHook.installAtSym<"_ZNK18DemoStateHackFirst20isEnableShowHackDemoEv">();
}
