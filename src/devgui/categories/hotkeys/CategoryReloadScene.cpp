#include "devgui/categories/hotkeys/CategoryReloadScene.h"

#include "Library/Nerve/NerveKeeper.h"
#include "al/Library/LiveActor/ActorMovementFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Nerve/NerveStateCtrl.h"

#include "game/Sequence/ChangeStageInfo.h"
#include "game/System/GameDataFile.h"
#include "game/System/GameDataFunction.h"

#include "custom/game/Scene/StageScene.h"

#include <cxxabi.h>
#include <typeinfo>

#include "helpers/GetHelper.h"
#include "helpers/PlayerHelper.h"
#include "imgui.h"

CategoryReloadScene::CategoryReloadScene(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryHotkeyBase(catName, catDesc, heap) {}

void CategoryReloadScene::updateCat() {
    CategoryHotkeyBase::updateCat();
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();

    // check if player exists
    if (!player)
        return;

    // stage reload code if position reloading is enabled (has to run in a constant loop)
    if (reloadStageForPos != -1) {
        reloadStageForPos++;
        if (reloadStageForPos == 11) {
            player->startDemoPuppetable();
            al::setTrans(player, reloadStageTrans);
            al::updatePoseQuat(player, reloadStageQuat);
            al::setVelocityZero(player);
            player->endDemoPuppetable();
            reloadStageForPos = -1;
        }
    }
}

void CategoryReloadScene::hotkeyFunc() {
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();
    StageScene* stageScene = tryGetStageScene();
    GameDataHolder* holder = tryGetGameDataHolder();

    // check if player exists
    if (!player || !stageScene || !holder)
        return;

    // reload stage at entrance if position reloading is disabled
    if (!mIsLoadPos) {
        StageScene* scene = tryGetStageScene();

        if (!scene->getNerveKeeper()->mStateCtrl)
            return;

        al::NerveKeeper* sceneNerveKeeper = scene->getNerveKeeper();
        const al::Nerve* sceneNerve = sceneNerveKeeper->getCurrentNerve();
        al::NerveStateCtrl::State* state = scene->getNerveKeeper()->mStateCtrl->findStateInfo(sceneNerve);
        if (state) {
            const al::Nerve* stateNerve = state->state->getNerveKeeper()->getCurrentNerve();
            int status;
            char* sceneName = abi::__cxa_demangle(typeid(*scene).name(), nullptr, nullptr, &status);
            char* stateName = abi::__cxa_demangle(typeid(*state->state).name(), nullptr, nullptr, &status);

            if (stateName && sceneName) {
                char* stateNrvName = abi::__cxa_demangle(typeid(*stateNerve).name(), nullptr, nullptr, &status);
                char* sceneNerveName = abi::__cxa_demangle(typeid(*sceneNerve).name(), nullptr, nullptr, &status);

                if (stateNrvName && sceneNerveName) {
                    auto prefixLen = sceneNerveName[0] == '(' ? strlen("(anonymous namespace)::") : 0;
                    char* stateNrvNameShort = stateNrvName + strlen("(anonymous namespace)::") + strlen(stateName) + strlen("nrv");
                    char* sceneNerveNameShort = sceneNerveName + prefixLen + strlen(sceneName) + strlen("nrv");
                    // strcmp(stateNrvName, "(anonymous namespace)::StageSceneStateTalkNrvSkipDemo")

                    if (strcmp(stateNrvNameShort, "SkipDemo") == 0 || strcmp(stateNrvNameShort, "Skip") == 0 ||
                        strcmp(sceneNerveNameShort, "Pause") == 0)
                        return;
                }
            }
        }
        scene->kill();
    }

    // reload stage at current position if position reloading is enabled
    if (mIsLoadPos) {
        reloadStageForPos = 0;
        if (player) {
            reloadStageTrans = al::getTrans(player);
            reloadStageQuat = al::getQuat(player);
        }
        const char* entry = holder->getGameDataFile()->getPlayerStartId();
        ChangeStageInfo info = ChangeStageInfo(holder, "start", GameDataFunction::getCurrentStageName(stageScene->mGameDataHolder), false, -1,
                                               ChangeStageInfo::SubScenarioType::NO_SUB_SCENARIO);
        holder->changeNextStage(&info, 0);
    }
}

void CategoryReloadScene::updateCatDisplay() {
    CategoryHotkeyBase::updateCatDisplay();

    if (!mIsOverride)
        return;

    // draw checkbox to enable/disable position reloading
    ImGui::Checkbox("Reload At Position", &mIsLoadPos);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Reloads the scene at the position\nyou are currently at\n(will \"break\" reload if disabled again)");
}
