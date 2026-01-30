#include "devgui/categories/hotkeys/CategoryReloadScene.h"

#include "Library/Nerve/NerveKeeper.h"
#include "al/Library/Base/StringUtil.h"
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
#include "helpers/InputHelper.h"
#include "helpers/PlayerHelper.h"
#include "imgui.h"

CategoryReloadScene::CategoryReloadScene(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryReloadScene::updateCat() {
    if (mIsOverride) {
        // check if keys are pressed
        if (al::isEqualString(mKey1, "None"))
            isKey1Pressed = true;
        if (al::isEqualString(mKey1, "ZL"))
            isKey1Pressed = InputHelper::isHoldZL();
        if (al::isEqualString(mKey1, "ZR"))
            isKey1Pressed = InputHelper::isHoldZR();
        if (al::isEqualString(mKey1, "L"))
            isKey1Pressed = InputHelper::isHoldL();
        if (al::isEqualString(mKey1, "R"))
            isKey1Pressed = InputHelper::isHoldR();
        if (al::isEqualString(mKey2, "None"))
            isKey2Pressed = true;
        if (al::isEqualString(mKey2, "ZL"))
            isKey2Pressed = InputHelper::isHoldZL();
        if (al::isEqualString(mKey2, "ZR"))
            isKey2Pressed = InputHelper::isHoldZR();
        if (al::isEqualString(mKey2, "L"))
            isKey2Pressed = InputHelper::isHoldL();
        if (al::isEqualString(mKey2, "R"))
            isKey2Pressed = InputHelper::isHoldR();

        // check if only enabled keys are pressed
        if ((InputHelper::isHoldZL() && !al::isEqualString(mKey1, "ZL") && !al::isEqualString(mKey2, "ZL")) ||
            (InputHelper::isHoldZR() && !al::isEqualString(mKey1, "ZR") && !al::isEqualString(mKey2, "ZR")) ||
            (InputHelper::isHoldL() && !al::isEqualString(mKey1, "L") && !al::isEqualString(mKey2, "L")) ||
            (InputHelper::isHoldR() && !al::isEqualString(mKey1, "R") && !al::isEqualString(mKey2, "R"))) {
            return;
        }
        // get needed stuff
        PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();
        StageScene* stageScene = tryGetStageScene();
        GameDataHolder* holder = tryGetGameDataHolder();
        GameDataHolderAccessor* accessor = tryGetGameDataHolderAccess();

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

        // reload stage at entrance if position reloading is disabled
        if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp() && !mIsLoadPos) {
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
        if (isKey1Pressed && isKey2Pressed && InputHelper::isPressPadUp() && mIsLoadPos) {
            reloadStageForPos = 0;
            if (player) {
                reloadStageTrans = al::getTrans(player);
                reloadStageQuat = al::getQuat(player);
            }
            const char* entry = accessor->mData->getGameDataFile()->getPlayerStartId();
            ChangeStageInfo info = ChangeStageInfo(holder, "start", GameDataFunction::getCurrentStageName(stageScene->mGameDataHolder), false, -1,
                                                   ChangeStageInfo::SubScenarioType::NO_SUB_SCENARIO);
            holder->changeNextStage(&info, 0);
        }
    }
}

void CategoryReloadScene::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    // draw checkbox to enable/disable the hotkey
    ImGui::Checkbox("Enable", &mIsOverride);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Enables the hotkey\nto reload the scene");

    if (!mIsOverride)
        return;

    // draw checkbox to enable/disable position reloading
    ImGui::Checkbox("Reload At Position", &mIsLoadPos);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Reloads the scene at the position\nyou are currently at\n(will \"break\" reload if disabled again)");

    // draw dropdowns to select the keys
    drawDropdown("Key 1", KeyNames, mTotalKeys, &mKey1);
    drawDropdown("Key 2", KeyNames, mTotalKeys, &mKey2);
}

void CategoryReloadScene::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output) {
    if (ImGui::BeginCombo(header, *output, mComboFlags)) {
        for (int n = 0; n < mTotalKeys; n++) {
            bool is_selected = (*output == options[n]);
            if (ImGui::Selectable(options[n], is_selected))
                *output = options[n];
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}
