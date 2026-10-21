#include "devgui/windows/WindowActorBrowse2.h"

#include "nn/hid.h"

#include "sead/gfx/seadColor.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/Clipping/FrustumRadar.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Library/LiveActor/LiveActor.h"
#include "al/Library/LiveActor/LiveActorGroup.h"
#include "al/Library/LiveActor/LiveActorInfo.h"
#include "al/Library/LiveActor/LiveActorKeeper.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Library/Model/ModelKeeper.h"
#include "al/Library/Projection/Projection.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Scene/SceneUtil.h"
#include "al/Library/Screen/ScreenFunction.h"
#include "al/Library/Sequence/Sequence.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/MapObj/GrowPlantSeed.h"
#include "game/Npc/FukuwaraiFaceParts.h"
#include "game/Npc/FukuwaraiWatcher.h"
#include "game/Player/PlayerHackKeeper.h"
#include "game/Sequence/HakoniwaSequence.h"

#include <cxxabi.h>
#include <typeinfo>

#include "devgui/DevGuiManager.h"
#include "devgui/popups/PopupKeyboard.h"
#include "devgui/windows/ActorBrowse2/ActorBrowseInspector.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "logger/Logger.hpp"
#include "primitives/PrimitiveQueue.h"

WindowActorBrowse2::WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault) {
    mFilterActorGroup = new (mHeap) al::LiveActorGroup("ActorBrowseFilter[LunaKit]", 8192);
}

void WindowActorBrowse2::updateWin() {
    WindowBase::updateWin();

    // Reset scene, even when display is closed
    al::Scene* scene = tryGetScene();
    if (!scene) {
        resetScene();
        return;
    }
    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        resetScene();
        return;
    }
    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        resetScene();
        return;
    }
}

template <typename T>
static const char* getClassName(const T* t) {
    static size_t demangleOutputLength = 0;
    static char* demangleOutputBuffer = nullptr;

    s32 demangleStatus;
    demangleOutputBuffer = abi::__cxa_demangle(typeid(*t).name(), demangleOutputBuffer, &demangleOutputLength, &demangleStatus);
    if (demangleStatus != 0)
        return nullptr;

    return demangleOutputBuffer;
}

bool WindowActorBrowse2::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    al::Scene* scene = tryGetScene();
    if (!scene) {
        ImGui::TextDisabled("No scene!");
        resetScene();
        return true;
    }

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        ImGui::TextDisabled("No LiveActorKit!");
        resetScene();
        return true;
    }

    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        ImGui::TextDisabled("No LiveActorGroup!");
        resetScene();
        return true;
    }

    mHoveredSensor = nullptr;
    mIsLayoutHorizontal = ImGui::GetWindowWidth() >= 600.0f;

    // In-world selector
    ImGui::SetNextItemWidth(200.0f);
    ImGui::DragFloat("##PickRadius", &mPickRadius, 10.0f, 10.0f, 1000.0f, "Radius: %.0f");

    ImGui::SameLine();
    if (!mIsPickingActor) {
        if (ImGui::Button("Pick Actor..."))
            mIsPickingActor = true;
    } else {
        if (ImGui::Button("Stop Picking"))
            mIsPickingActor = false;
    }

    // USB keyboard search
    ImGui::SetNextItemWidth(200.0f);
    if (ImGui::InputTextWithHint("##Filter", "Search...", mSearchFilter.InputBuf, sizeof(mSearchFilter.InputBuf),
                                 ImGuiInputTextFlags_EscapeClearsAll)) {
        mSearchText = mSearchFilter.InputBuf;
        mSearchFilter.Build();

        regenerateFilerActorGroup(group);
    }

    // Popup keyboard search
    ImGui::SameLine();
    if (mSearchFilter.IsActive()) {
        if (ImGui::Button("Clear")) {
            mSearchText = nullptr;
            mSearchFilter.Clear();
        }
    } else if (ImGui::Button("Open Keyboard...")) {
        mParent->tryOpenKeyboard(sizeof(mSearchFilter.InputBuf), PopupKeyboardType::KEYTYPE_QWERTY, &mSearchText, &mIsPopupKeyboardOpen);
    }

    if (mIsPopupKeyboardOpen && mSearchText != nullptr) {
        strncpy(mSearchFilter.InputBuf, mSearchText, sizeof(mSearchFilter.InputBuf));
        mSearchFilter.Build();

        regenerateFilerActorGroup(group);
    }

    displayActorList(group);
    if (mIsLayoutHorizontal)
        ImGui::SameLine();
    displayActorInspector(scene, mSelectedActor);

    auto primQueue = mParent->getPrimitiveQueue();

    if (mIsPickingActor) {
        // Draw actor positions
        const auto& camera = al::getLookAtCamera(scene, 0);
        const auto& projection = al::getProjection(scene, 0);
        al::FrustumRadar radar;
        radar.calcFrustumArea(camera.getMatrix(), projection.getMtxStd(), al::getNear(scene, 0), al::getFar(scene, 0));

        sead::Vector2f targetScreenPos;
        InputHelper::getMouseCoords(&targetScreenPos.x, &targetScreenPos.y);
        sead::Vector3f targetWorldPos;
        al::calcWorldPosFromScreenPos(&targetWorldPos, scene, targetScreenPos, 1.0f);

        sead::Vector3f worldDir = camera.getPos() - targetWorldPos;
        worldDir.normalize();

        s32 hitIdx = 0, pickIdx = mPickHitIdx;

        for (s32 idx = 0; idx < group->getActorCount(); idx++) {
            auto actor = group->getActor(idx);
            auto pose = actor->getPoseKeeper();
            if (!pose)
                continue;
            if (!radar.judgeInArea(pose->getTrans(), mPickRadius))
                continue;

            sead::Vector3f camDelta = pose->getTrans() - camera.getPos();
            if (camDelta.squaredLength() <= mPickRadius * mPickRadius)
                continue;
            if (camDelta.squaredLength() >= cMaxPickDistance * cMaxPickDistance)
                continue;

            // Ray/Sphere intersection
            sead::Vector3f rayDelta;
            rayDelta.setCross(camDelta, worldDir);

            bool hover = rayDelta.squaredLength() <= mPickRadius * mPickRadius;
            bool target = hitIdx == mPickHitIdx;

            if (!primQueue->isFull())
                primQueue->pushPoint(pose->getTrans(), mPickRadius,
                                     (hover && target) ? sead::Color4f(0.0f, 1.0f, 0.0f, 0.25f) : sead::Color4f(1.0f, 0.0f, 0.0f, 0.25f));

            if (hover) {
                if (target && ImGui::BeginTooltip()) {
                    auto model = actor->getModelKeeper();

                    const char* className = getClassName(actor);
                    const char* actorName = actor->getName();
                    const char* modelName = model ? model->getName() : nullptr;

                    if (mPickHitNum > 1) {
                        ImGui::Text("Use the scroll wheel to cycle through overlapping actors");
                        ImGui::Text("Currently selected: %d / %d", mPickHitIdx + 1, mPickHitNum);
                        ImGui::Separator();

                        f32 scrollX, scrollY;
                        InputHelper::getScrollDelta(&scrollX, &scrollY);

                        if (scrollY > 0.0f) {
                            if (!mDidChangePickIdx)
                                pickIdx = al::wrapValue(mPickHitIdx + 1, mPickHitNum);
                            mDidChangePickIdx = true;
                        } else if (scrollY < 0.0f) {
                            if (!mDidChangePickIdx)
                                pickIdx = al::wrapValue(mPickHitIdx - 1, mPickHitNum);
                            mDidChangePickIdx = true;
                        } else {
                            mDidChangePickIdx = false;
                        }
                    }

                    if (className)
                        ImGui::Text("Class: %s", className);
                    if (actorName)
                        ImGui::Text("Actor: %s", actorName);
                    if (modelName)
                        ImGui::Text("Model: %s", modelName);

                    ImGui::EndTooltip();
                }
                if (target && InputHelper::isMousePress(nn::hid::MouseButton::Left) && !ImGui::GetIO().WantCaptureMouse) {
                    mSelectedActor = actor;
                    mIsPickingActor = false;
                    mShouldScrollToSelectedActor = true;
                    break;
                }

                hitIdx++;
            }
        }

        mPickHitNum = hitIdx;
        mPickHitIdx = al::wrapValue(pickIdx, mPickHitNum);

    } else {
        // Draw selected position
        auto pose = mSelectedActor ? mSelectedActor->getPoseKeeper() : nullptr;
        if (pose)
            primQueue->pushPoint(pose->getTrans(), 10.0f, sead::Color4f(0.25f, 0.5f, 0.5f, 0.25f));

        // Draw sensors
        if (mShouldRenderAllSensors && mSelectedActor)
            primQueue->pushHitSensorKeeper(mSelectedActor->getHitSensorKeeper(), HitSensorType_ALL, 0.4f);
        else if (mHoveredSensor)
            primQueue->pushHitSensor(mHoveredSensor, 0.8f);

        for (s32 idx = 0; idx < mRenderedSensors.size(); idx++) {
            auto sensor = mRenderedSensors[idx];
            if (mShouldRenderAllSensors && sensor->getParentActor() == mSelectedActor)
                continue;
            if (sensor == mHoveredSensor)
                continue;

            primQueue->pushHitSensor(sensor, 0.4f);
        }
    }

    return true;
}

void WindowActorBrowse2::resetScene() {
    mSelectedActor = nullptr;
    mHoveredSensor = nullptr;
    mPinnedActors.clear();
    mRenderedSensors.clear();

    mIsPickingActor = false;
}

void WindowActorBrowse2::displayActorList(al::LiveActorGroup* group) {
    ImVec2 size = mIsLayoutHorizontal ? ImVec2(400.0f, 0.0f) : ImVec2(0.0f, 350.0f);
    ImGuiChildFlags childFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened;
    if (mIsLayoutHorizontal)
        childFlags |= ImGuiChildFlags_ResizeX;
    else
        childFlags |= ImGuiChildFlags_ResizeY;

    if (mIsLayoutHorizontal)
        ImGui::SetNextWindowSizeConstraints(ImVec2(200.0f, 0.0f), ImVec2(ImGui::GetContentRegionAvail().x - 200.0f, FLT_MAX));
    else
        ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 150.0f), ImVec2(FLT_MAX, ImGui::GetContentRegionAvail().y - 150.0f));

    // Separate IDs to retain user-edited sizes
    const char* strID = mIsLayoutHorizontal ? "##ListHorizontal" : "##ListVertical";

    if (ImGui::BeginChild(strID, size, childFlags)) {
        auto shownGroup = mSearchFilter.IsActive() ? mFilterActorGroup : group;

        if (ImGui::BeginTable("##Table", 1, ImGuiTableFlags_RowBg)) {
            ImGuiListClipper clipper;
            clipper.Begin(INT_MAX);

            for (s32 idx = 0; idx < mPinnedActors.size(); idx++)
                displayActorListEntry(mPinnedActors[idx], clipper);

            for (s32 idx = 0; idx < shownGroup->getActorCount(); idx++) {
                auto actor = shownGroup->getActor(idx);
                // Pinned actors have already been rendered, so don't render them again
                if (mPinnedActors.indexOf(actor) == -1)
                    displayActorListEntry(actor, clipper);
            }

            clipper.SeekCursorForItem(clipper.UserIndex);

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

void WindowActorBrowse2::displayActorListEntry(al::LiveActor* actor, ImGuiListClipper& clipper) {
    auto model = actor->getModelKeeper();
    auto subActors = actor->getSubActorKeeper();

    // Advance the clipper until we have data again or it's done
    while (clipper.UserIndex >= clipper.DisplayEnd)
        if (!clipper.Step())
            break;

    ImGuiID uid = (ImGuiID)(uintptr_t)actor;

    bool drawSubActors = false;
    if ((clipper.UserIndex >= clipper.DisplayStart && clipper.UserIndex < clipper.DisplayEnd) || mShouldScrollToSelectedActor) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_None;
        treeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        treeFlags |= ImGuiTreeNodeFlags_NavLeftJumpsToParent;
        treeFlags |= ImGuiTreeNodeFlags_SpanFullWidth;
        treeFlags |= ImGuiTreeNodeFlags_DrawLinesToNodes;

        const char* className = getClassName(actor);
        const char* actorName = actor->getName();
        const char* modelName = model ? model->getName() : nullptr;

        if (!className)
            return;

        if (actor == mSelectedActor)
            treeFlags |= ImGuiTreeNodeFlags_Selected;
        if (!subActors || subActors->getCurActorCount() == 0)
            treeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;

        ImGui::SetNextItemStorageID(uid);

        if (modelName && actorName)
            drawSubActors = ImGui::TreeNodeEx(actor, treeFlags, "%s (%s) [%s]", className, actorName, modelName);
        else if (actor->getName())
            drawSubActors = ImGui::TreeNodeEx(actor, treeFlags, "%s (%s)", className, actorName);
        else
            drawSubActors = ImGui::TreeNodeEx(actor, treeFlags, "%s", className);

        if (!subActors || subActors->getCurActorCount() == 0)
            drawSubActors = false;

        if (mShouldScrollToSelectedActor && actor == mSelectedActor) {
            Logger::log("Scrolling to %p at %.1f, %.1f", actor, ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
            ImGui::ScrollToItem(ImGuiScrollFlags_AlwaysCenterY);
            mShouldScrollToSelectedActor = false;
        }

        if (ImGui::IsItemFocused())
            mSelectedActor = actor;
    } else {
        drawSubActors = subActors && subActors->getCurActorCount() > 0 && ImGui::TreeNodeGetOpen(uid);
        if (drawSubActors)
            ImGui::TreePush(actor);
    }

    clipper.UserIndex++;

    if (drawSubActors) {
        for (s32 idx = 0; idx < subActors->getCurActorCount(); idx++)
            displayActorListEntry(subActors->getActorInfo(idx)->subActor, clipper);

        ImGui::TreePop();
    }
}

void WindowActorBrowse2::regenerateFilerActorGroup(const al::LiveActorGroup* group) {
    mFilterActorGroup->removeActorAll();

    for (s32 idx = 0; idx < group->getActorCount(); idx++) {
        auto actor = group->getActor(idx);
        if (checkActorMatchesFilter(actor))
            mFilterActorGroup->registerActor(actor);
    }
}

bool WindowActorBrowse2::checkActorMatchesFilter(const al::LiveActor* actor) const {
    auto model = actor->getModelKeeper();
    auto subActors = actor->getSubActorKeeper();

    const char* className = getClassName(actor);
    const char* actorName = actor->getName();
    const char* modelName = model ? model->getName() : nullptr;

    if (mSearchFilter.PassFilter(className) || mSearchFilter.PassFilter(actorName) || mSearchFilter.PassFilter(modelName))
        return true;

    if (subActors) {
        for (s32 idx = 0; idx < subActors->getCurActorCount(); idx++)
            if (checkActorMatchesFilter(subActors->getActorInfo(idx)->subActor))
                return true;
    }

    return false;
}

void WindowActorBrowse2::displayActorInspector(al::Scene* scene, al::LiveActor* actor) {
    if (!actor) {
        ImGui::TextDisabled("No Actor selected!");
        return;
    }

    if (ImGui::BeginChild("##Inspector", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened)) {
        s32 pinIndex = mPinnedActors.indexOf(actor);
        if (pinIndex >= 0) {
            if (ImGui::Button("Unpin from top"))
                mPinnedActors.erase(pinIndex);
        } else {
            ImGui::BeginDisabled(mPinnedActors.isFull());

            if (ImGui::Button("Pin to top") && !mPinnedActors.isFull())
                mPinnedActors.pushBack(actor);

            if (mPinnedActors.isFull())
                ImGui::SetTooltip("Reached maximum amount of %d pinned actors", mPinnedActors.capacity());

            ImGui::EndDisabled();
        }

        auto player = tryGetPlayerActor();
        if (player && player->getPoseKeeper() && actor->getPoseKeeper()) {
            ImGui::SameLine();
            if (ImGui::Button("Teleport to Actor")) {
                player->startDemoPuppetable();
                player->getPoseKeeper()->mTrans = actor->getPoseKeeper()->mTrans;
                player->endDemoPuppetable();
            }
        }

        if (al::isEqualString(typeid(*actor).name(), typeid(FukuwaraiFaceParts).name()))
            mActorInspectorFukuwaraiFaceParts.display(this, scene, actor);
        else if (al::isEqualString(typeid(*actor).name(), typeid(FukuwaraiWatcher).name()))
            mActorInspectorFukuwaraiWatcher.display(this, scene, actor);
        else if (al::isEqualString(typeid(*actor).name(), typeid(GrowPlantSeed).name()))
            mActorInspectorGrowPlantSeed.display(this, scene, actor);
        else
            mActorInspectorDefault.display(this, scene, actor);
    }
    ImGui::EndChild();
}
