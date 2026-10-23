#include "info/InfoProviderGrowSeed.h"

#include "hk/ro/RoUtil.h"

#include "game/MapObj/GrowPlantSeed.h"
#include "game/MapObj/GrowSeedStateWait.h"

#include "MapObj/GrowPlantSeed.h"
#include "helpers/NrvHelper.h"
#include "info/InfoProviderActor.h"

void InfoProviderGrowPlantSeed::displayImpl() {
    InfoProviderActor::displayImpl();

    auto seed = reinterpret_cast<GrowPlantSeed*>(mActor);

    // v1.2.0 has patched Nut Jumping by checking the currently activate nerve
    bool canTrample;
    if (hk::ro::getMainModule()->isVersion("120")) {
        canTrample = seed->mTrampleCooldown == 0 && ((NrvHelper::isNerveSym<"GrowPlantSeedNrvWait">(seed) && seed->mWait->isConnecting()) ||
                                                     (NrvHelper::isNerveSym<"GrowPlantSeedNrvWaitWater">(seed) && seed->mWaitWater->isConnecting()));
    } else {
        canTrample = seed->mTrampleCooldown == 0 && (seed->mWait->isConnecting() || seed->mWaitWater->isConnecting());
    }

    ImGui::Text("Can Bounce: %s", canTrample ? "True" : "False");
}
