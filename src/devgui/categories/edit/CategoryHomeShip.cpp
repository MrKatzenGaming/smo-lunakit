#include "devgui/categories/edit/CategoryHomeShip.h"

#include "hk/ro/RoUtil.h"

#include "game/System/GameDataFile.h"
#include "game/System/GameProgressData.h"

#include "helpers/GetHelper.h"
#include "helpers/helpers.h"
#include "imgui.h"

CategoryHomeShip::CategoryHomeShip(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {
    mUpdateStatusListSize = IM_ARRAYSIZE(updateStatusList);
    mUpdateWaterfallListSize = IM_ARRAYSIZE(updateWaterfallList);
}

void CategoryHomeShip::updateCat() {
    GameDataHolder* holder = tryGetGameDataHolder();
    if (!holder)
        return;

    GameProgressData* progress = nullptr;
    if (hk::ro::getMainModule()->isVersion("120"))
        progress = *(&holder->getGameDataFile()->mGameProgressData - 1);
    else
        progress = holder->getGameDataFile()->mGameProgressData;

    if (mIsUpdateWorld) {
        mIsUpdateWorld = false;
        progress->mUnlockWorldNum = mUpdateWorldIdx;
    }

    if (mIsUpdateStatus) {
        mIsUpdateStatus = false;
        progress->mHomeStatus = (GameProgressData::HomeStatus)mUpdateWorldIdx;
    }

    if (mIsUpdateLevel) {
        mIsUpdateLevel = false;
        progress->mHomeLevel = mUpdateLevelIdx;
    }

    if (mIsUpdateWaterfall) {
        mIsUpdateWaterfall = false;
        progress->mWaterfallWorldProgress = (GameProgressData::WaterfallWorldProgress)mUpdateWorldIdx;
    }
}

void CategoryHomeShip::updateCatDisplay() {
    CategoryBase::updateCatDisplay();

    GameDataHolder* holder = tryGetGameDataHolder();
    if (!holder)
        return;

    GameProgressData* progress = nullptr;
    if (hk::ro::getMainModule()->isVersion("120"))
        progress = *(&holder->getGameDataFile()->mGameProgressData - 1);
    else
        progress = holder->getGameDataFile()->mGameProgressData;
    ;

    if (ImGui::BeginTabBar("Sub-categories")) {
        if (ImGui::BeginTabItem("Worlds")) {
            if (mUpdateWorldIdx == -1)
                mUpdateWorldIdx = progress->mUnlockWorldNum;

            drawSelectionIndex("World Count", &mUpdateWorldIdx, &mIsUpdateWorld, ImVec2(1, 17));
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip("Total unlocked kingdoms that\nthe Odyssey ship can visit");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Status")) {
            if (mUpdateStatusIdx == -1)
                mUpdateStatusIdx = (int)progress->mHomeStatus;

            drawSelectionList("Status", &mUpdateStatusIdx, &mIsUpdateStatus, updateStatusList, mUpdateStatusListSize);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Level")) {
            if (mUpdateLevelIdx == -1)
                mUpdateLevelIdx = progress->mHomeLevel;

            drawSelectionIndex("Level", &mUpdateLevelIdx, &mIsUpdateLevel, ImVec2(0, 9));
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip("Total segments of Odyssey's\nlarge balloon");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Waterfall")) {
            if (mUpdateWaterfallIdx == -1)
                mUpdateWaterfallIdx = (int)progress->mWaterfallWorldProgress;

            drawSelectionList("Waterfall", &mUpdateWaterfallIdx, &mIsUpdateWaterfall, updateWaterfallList, mUpdateWaterfallListSize);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip("Some Cascade Kingdom cutscene\ntriggers use this progression info");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void CategoryHomeShip::drawSelectionIndex(const char* header, int* idx, bool* activation, ImVec2 minmax) {
    ImGui::BeginGroup();

    ImGui::SliderInt(header, idx, minmax.x, minmax.y);
    if (ImGui::Button("Set Parameter"))
        *activation = true;

    ImGui::EndGroup();
}

void CategoryHomeShip::drawSelectionList(const char* header, int* idx, bool* activation, const char* list[], int listSize) {
    ImGui::BeginGroup();

    const char* currentString = list[*idx];

    if (ImGui::BeginCombo(header, currentString)) {
        for (int n = 0; n < listSize; n++) {
            bool is_selected = (currentString == list[n]);  // You can store your selection however you want, outside or inside your objects

            if (ImGui::Selectable(list[n], is_selected))
                *idx = n;

            if (is_selected)
                ImGui::SetItemDefaultFocus();  // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Set Parameter"))
        *activation = true;

    ImGui::EndGroup();
}
