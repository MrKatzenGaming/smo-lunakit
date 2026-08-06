#include "devgui/DevGuiHooks.h"

#include "devgui/settings/SettingsHooks.h"
#include "devgui/windows/StagePause/StagePauseHooks.h"
#include "devgui/windows/TASTools/WindowTASTools.h"

void DevGuiHooks::exlInstallDevGuiHooks() {
    exlSetupSettingsHooks();
    // exlSetupGraphicsHooks();
    exlSetupStageSceneHooks();
    setupTasHooks();
}
