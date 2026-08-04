#include "devgui/homemenu/HomeMenuWorlds.h"

#include "hk/ro/RoUtil.h"

#include "al/Library/Base/StringUtil.h"

#include "game/MapObj/ChangeStageInfo.h"
#include "game/Player/PlayerFunction.h"
#include "game/System/GameDataFunction.h"

#include "System/WorldList.h"
#include "devgui/DevGuiManager.h"
#include "helpers/GetHelper.h"
#include "imgui.h"

HomeMenuWorlds::HomeMenuWorlds(DevGuiManager* parent, const char* menuName, bool isDisplayInListByDefault)
    : HomeMenuBase(parent, menuName, isDisplayInListByDefault) {}

void HomeMenuWorlds::updateMenuDisplay() {
    StageScene* scene = tryGetStageScene();
    if (!scene) {
        mScenarioPicker = -1;
        ImGui::MenuItem("No stage is loaded!", nullptr, false, false);
        return;
    }

    GameDataHolderAccessor holder(scene);
    s32 worldID = GameDataFunction::getCurrentWorldId(holder);
    const char* worldName = GameDataFunction::getWorldDevelopName(holder, worldID);

    // Draw the top components to select kindom and scenario
    drawKingdomPicker(worldName, holder);
    if (GameDataFunction::isMainStage(holder))
        drawScenarioPicker(mainNames[worldID], scene);

    if (addMenu("Sub-Areas")) {
        if (hk::ro::getMainModule()->isVersion("120")) {
            ImGui::MenuItem("Not available on 1.2!", nullptr, false, false);
            ImGui::EndMenu();
            return;
        }
        ImGui::BeginChild("Sub-Area Child", ImVec2(525, 325), false, ImGuiWindowFlags_NoBackground);

        for (auto& dbEntry : holder->getWorldList()->mWorldList[worldID]->stageList) {
            bool isDemo = al::isEqualString(dbEntry.category, "Demo");

            if (ImGui::MenuItem(dbEntry.name.cstr(), dbEntry.name.cstr(), false, !isDemo))
                warpToStage(holder, dbEntry.name.cstr(), dbEntry.useScenarioNo);
        }

        ImGui::EndChild();
        ImGui::EndMenu();
    }
}

inline void HomeMenuWorlds::drawKingdomPicker(const char* worldName, GameDataHolderAccessor holder) {
    ImGui::SetNextItemWidth(140.f);
    if (ImGui::BeginCombo("Kingdom", worldName, ImGuiComboFlags_HeightLargest)) {
        ImGui::SetWindowFontScale(1.66f);

        for (auto& entry : mainNames)
            if (ImGui::Selectable(entry.mShort, false))
                warpToStage(holder, entry.mInternal, -1);

        ImGui::EndCombo();
    }
}

inline void HomeMenuWorlds::drawScenarioPicker(KingdomEnglishNameMain& entry, StageScene* scene) {
    PlayerActorBase* player = tryGetPlayerActor(scene);
    if (player && mScenarioPicker == -1)
        mScenarioPicker = GameDataFunction::getScenarioNo(player);

    ImGui::Text("Scenario (%s)", getScenarioType(entry, mScenarioPicker));

    ImGui::SetNextItemWidth(60.f);
    ImGui::DragInt("\n\0", &mScenarioPicker, 0.075f, 1, 15, nullptr, ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoInput);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Click and drag to change the scenario!\nClick load to reload the kingdom with this scenario");

    ImGui::SameLine();
    if (ImGui::Button(" Load "))
        warpToStage(scene, GameDataFunction::getCurrentStageName(scene), mScenarioPicker);
}

inline const char* HomeMenuWorlds::getScenarioType(KingdomEnglishNameMain& entry, int scenario) {
    if (scenario == entry.clearMainScenario)
        return "Peace";
    if (scenario == entry.endingScenario)
        return "Post-game";
    if (scenario == entry.moonRockScenario)
        return "Moon Rock";

    return "Unknown";
}

void HomeMenuWorlds::warpToStage(GameDataHolderAccessor data, const char* stageName, int scenario) {
    PlayerActorBase* player = tryGetPlayerActor();
    if (!player)
        return;

    if (PlayerFunction::isPlayerDeadStatus(player))
        return;

    ChangeStageInfo stageInfo(data.mData, "start", stageName, false, scenario, ChangeStageInfo::SubScenarioType::NO_SUB_SCENARIO);
    GameDataFunction::tryChangeNextStage(data.mData, &stageInfo);
}
