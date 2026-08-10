#include "devgui/DevGuiHooks.h"

#include "devgui/settings/SettingsHooks.h"

void DevGuiHooks::exlInstallDevGuiHooks() {
    exlSetupSettingsHooks();
    exlSetupStageSceneHooks();
    setupTasHooks();
}
