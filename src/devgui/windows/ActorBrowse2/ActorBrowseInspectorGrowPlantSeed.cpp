
#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/MapObj/GrowPlantSeed.h"
#include "game/MapObj/GrowSeedStateWait.h"
#include "game/Player/PlayerHackKeeper.h"

#include "devgui/windows/ActorBrowse2/ActorBrowseInspector.h"
#include "imgui.h"

void ActorBrowseInspectorGrowPlantSeed::display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) {
    ActorBrowseInspectorBase::display(window, scene, actor);

    auto seed = reinterpret_cast<GrowPlantSeed*>(actor);

    // TODO: Use patched check for v1.2.0
    bool canTrample = seed->mTrampleCooldown == 0 && (seed->mWait->isConnecting() || seed->mWaitWater->isConnecting());
    ImGui::Text("Can Bounce: %s", canTrample ? "True" : "False");
}
