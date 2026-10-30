#include "devgui/categories/actor/CategoryActorList.h"

#include "sead/heap/seadDisposer.h"

#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/Clipping/FrustumRadar.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/LiveActorInfo.h"
#include "al/Library/LiveActor/LiveActorKeeper.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Library/Model/ModelKeeper.h"
#include "al/Library/Projection/Projection.h"
#include "al/Library/Screen/ScreenFunction.h"

#include <cxxabi.h>

#include "devgui/DevGuiManager.h"
#include "devgui/categories/CategoryBase.h"
#include "devgui/categories/actor/CategoryActorInspectors.h"
#include "devgui/windows/WindowActorBrowse2.h"
#include "helpers/GetHelper.h"
#include "helpers/InputHelper.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "info/InfoProviderActor.h"
#include "logger/Logger.hpp"
#include "primitives/PrimitiveQueue.h"

static const char* demangleTypeName(const char* mangledName) {
    static size_t length = 0;
    static char* buffer = nullptr;

    s32 demangleStatus;
    buffer = abi::__cxa_demangle(mangledName, buffer, &length, &demangleStatus);
    if (demangleStatus != 0)
        return nullptr;

    return buffer;
}

CategoryActorList::CategoryActorList(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {
    mFilterActorGroup = new (mHeap) al::LiveActorGroup("ActorBrowseFilter[LunaKit]", 8192);
}

void CategoryActorList::updateCat() {
    if (mSelectedActor && !mIsPickingActor)
        InfoProviderActor::renderActor(InfoProviderBase::RenderInfo{.renderSelf = true, .forceRenderChildren = false}, mSelectedActor);
}

void CategoryActorList::updateCatDisplay() {
    al::Scene* scene = tryGetScene();
    if (!scene) {
        ImGui::TextDisabled("No scene!");
        return;
    }

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        ImGui::TextDisabled("No LiveActorKit!");
        return;
    }

    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        ImGui::TextDisabled("No LiveActorGroup!");
        return;
    }

    // mHoveredSensor = nullptr;
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
        SEAD_INSTANCE(DevGuiManager)
            ->tryOpenKeyboard(sizeof(mSearchFilter.InputBuf), PopupKeyboardType::KEYTYPE_QWERTY, &mSearchText, &mIsPopupKeyboardOpen);
    }

    if (mIsPopupKeyboardOpen && mSearchText != nullptr) {
        strncpy(mSearchFilter.InputBuf, mSearchText, sizeof(mSearchFilter.InputBuf));
        mSearchFilter.Build();

        regenerateFilerActorGroup(group);
    }

    auto primQueue = SEAD_INSTANCE(DevGuiManager)->getPrimitiveQueue();

    displayActorList(group);
    if (mIsLayoutHorizontal)
        ImGui::SameLine();
    displayActorInspector(mSelectedActor);

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

                    InfoProviderActor(actor).displayName();

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
    }

    return;
}

void CategoryActorList::resetScene() {
    mSelectedActor = nullptr;
    mPinnedActors.clear();

    mIsPickingActor = false;
}

void CategoryActorList::displayActorList(al::LiveActorGroup* group) {
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
        ImGui::SetWindowFontScale(DevGuiManager::cWindowFontScale);

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

void CategoryActorList::displayActorListEntry(al::LiveActor* actor, ImGuiListClipper& clipper) {
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

        const char* className = demangleTypeName(typeid(*actor).name());
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

void CategoryActorList::regenerateFilerActorGroup(const al::LiveActorGroup* group) {
    mFilterActorGroup->removeActorAll();

    for (s32 idx = 0; idx < group->getActorCount(); idx++) {
        auto actor = group->getActor(idx);
        if (checkActorMatchesFilter(actor))
            mFilterActorGroup->registerActor(actor);
    }
}

bool CategoryActorList::checkActorMatchesFilter(const al::LiveActor* actor) const {
    auto model = actor->getModelKeeper();
    auto subActors = actor->getSubActorKeeper();

    const char* className = demangleTypeName(typeid(*actor).name());
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

void CategoryActorList::displayActorInspector(al::LiveActor* actor) {
    if (!actor) {
        ImGui::TextDisabled("No Actor selected!");
        return;
    }

    if (ImGui::BeginChild("##Inspector", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened)) {
        ImGui::SetWindowFontScale(DevGuiManager::cWindowFontScale);

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

        InfoProviderActor::displayActor(actor);
    }
    ImGui::EndChild();
}
