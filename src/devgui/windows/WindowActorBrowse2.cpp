#include "devgui/windows/WindowActorBrowse2.h"

#include "Library/LiveActor/ActorPoseKeeper.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorGroup.h"
#include "Library/Nerve/NerveStateCtrl.h"
#include "al/Library/Base/StringUtil.h"
#include "al/Library/LiveActor/LiveActorInfo.h"
#include "al/Library/LiveActor/LiveActorKeeper.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Model/ModelKeeper.h"
#include "al/Library/Scene/Scene.h"
#include "al/Library/Scene/SceneUtil.h"
#include "al/Library/Sequence/Sequence.h"

#include "game/Sequence/HakoniwaSequence.h"
#include "game/System/GameSystem.h"

#include <cxxabi.h>
#include <typeinfo>

#include "devgui/DevGuiManager.h"
#include "devgui/popups/PopupKeyboard.h"
#include "helpers/GetHelper.h"
#include "helpers/ImGuiHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "primitives/PrimitiveQueue.h"

WindowActorBrowse2::WindowActorBrowse2(DevGuiManager* parent, const char* winName, bool isActiveByDefault)
    : WindowBase(parent, winName, isActiveByDefault) {
    mFilterActorGroup = new (mHeap) al::LiveActorGroup("ActorBrowseFilter[LunaKit]", 8192);
}

void WindowActorBrowse2::updateWin() {
    WindowBase::updateWin();
}

bool WindowActorBrowse2::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay())
        return false;

    al::Scene* scene = tryGetScene();
    if (!scene) {
        ImGui::TextDisabled("No scene!");
        mSelectedActor = nullptr;
        return true;
    }

    al::LiveActorKit* kit = scene->mLiveActorKit;
    if (!kit) {
        ImGui::TextDisabled("No LiveActorKit!");
        mSelectedActor = nullptr;
        return true;
    }

    al::LiveActorGroup* group = kit->mLiveActorGroupAllActors;
    if (!group) {
        ImGui::TextDisabled("No LiveActorGroup!");
        mSelectedActor = nullptr;
        return true;
    }

    displayActorList(group);
    ImGui::SameLine();
    displayActorInspector(mSelectedActor);

    return true;
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

void WindowActorBrowse2::displayActorList(al::LiveActorGroup* group) {
    if (ImGui::BeginChild("##List", ImVec2(500, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened)) {
        if (ImGui::InputTextWithHint("##Filter", "Search...", mSearchFilter.InputBuf, sizeof(mSearchFilter.InputBuf),
                                     ImGuiInputTextFlags_EscapeClearsAll)) {
            mSearchText = mSearchFilter.InputBuf;
            mSearchFilter.Build();

            regenerateFilerActorGroup(group);
        }

        ImGui::SameLine();
        if (mSearchFilter.IsActive()) {
            if (ImGui::Button("Clear")) {
                mSearchText = nullptr;
                mSearchFilter.Clear();
            }
        } else if (ImGui::Button("Open Keyboard")) {
            mParent->tryOpenKeyboard(sizeof(mSearchFilter.InputBuf), PopupKeyboardType::KEYTYPE_QWERTY, &mSearchText, &mKeyboardOpen);
        }

        if (mKeyboardOpen && mSearchText != nullptr) {
            strncpy(mSearchFilter.InputBuf, mSearchText, sizeof(mSearchFilter.InputBuf));
            mSearchFilter.Build();

            regenerateFilerActorGroup(group);
        }

        auto shownGroup = mSearchFilter.IsActive() ? mFilterActorGroup : group;

        if (ImGui::BeginTable("##Table", 1, ImGuiTableFlags_RowBg)) {
            ImGuiListClipper clipper;
            clipper.Begin(INT_MAX);

            for (s32 idx = 0; idx < shownGroup->getActorCount(); idx++)
                displayActorListEntry(shownGroup->getActor(idx), clipper);

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
    if (clipper.UserIndex >= clipper.DisplayStart && clipper.UserIndex < clipper.DisplayEnd) {
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

void WindowActorBrowse2::displayActorInspector(al::LiveActor* actor) {
    if (!actor) {
        ImGui::TextDisabled("No Actor selected!");
        return;
    }

    if (ImGui::BeginChild("##Inspector")) {
        displayActorInspectorTreeName(mSelectedActor);
        displayActorInspectorTreePose(mSelectedActor->mPoseKeeper);
        displayActorInspectorTreeFlags(mSelectedActor->mFlags);
        displayActorInspectorTreeNrv(mSelectedActor->getNerveKeeper());
        displayActorInspectorTreeSensor(mSelectedActor->mHitSensorKeeper);
    }
    ImGui::EndChild();
}

void WindowActorBrowse2::displayActorInspectorTreeName(al::LiveActor* actor) {
    if (!actor)
        return;
    if (!ImGui::TreeNode("Name"))
        return;

    auto model = actor->getModelKeeper();

    const char* className = getClassName(actor);
    const char* actorName = actor->getName();
    const char* modelName = model ? model->getName() : nullptr;

    if (className)
        ImGui::Text("Class: %s", className);
    if (actorName)
        ImGui::Text("Actor: %s", actorName);
    if (modelName)
        ImGui::Text("Model: %s", modelName);

    ImGui::TreePop();
}

void WindowActorBrowse2::displayActorInspectorTreePose(al::ActorPoseKeeperBase* poseKeep) {
    if (!poseKeep)
        return;
    if (!ImGui::TreeNode("Pose"))
        return;
    defer {
        ImGui::TreePop();
    };

    ImGuiHelper::Vector3Drag("Translation", "Pose Keeper Translation", poseKeep->getTransPtr(), 50.f, 0.f);
    ImGuiHelper::Vector3Drag("Scale", "Pose Keeper Scale", poseKeep->getScalePtr(), 0.05f, 0.f);
    ImGuiHelper::Vector3Drag("Velocity", "Pose Keeper Velocity", poseKeep->getVelocityPtr(), 1.f, 0.f);
    ImGuiHelper::Vector3Slide("Front", "Pose Keeper Front", poseKeep->getFrontPtr(), 1.f, true);
    ImGuiHelper::Vector3Slide("Up", "Pose Keeper Up", poseKeep->getUpPtr(), 1.f, true);
    ImGuiHelper::Vector3Slide("Gravity", "Pose Keeper Gravity", poseKeep->getGravityPtr(), 1.f, true);
    ImGuiHelper::Vector3Drag("Rotation", "Pose Keeper Rotation", poseKeep->getRotatePtr(), 1.f, 360.f);
    ImGuiHelper::QuatDrag("Quaternion", "Pose Keeper Quaternion", poseKeep->getQuatPtr());
}

void WindowActorBrowse2::displayActorInspectorTreeFlags(al::LiveActorFlag* flags) {
    if (!flags)
        return;
    if (!ImGui::TreeNode("Flags"))
        return;
    defer {
        ImGui::TreePop();
    };

    if (!ImGui::BeginTable("##Table", 2))
        return;
    defer {
        ImGui::EndTable();
    };

    ImGui::TableNextColumn();
    ImGui::Checkbox("Dead", &flags->isDead);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Clipped", &flags->isClipped);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Clipping Invalid", &flags->isClippingInvalid);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Draw Clipped", &flags->isDrawClipped);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Disable Calc Anim", &flags->isDisableCalcAnim);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Model Hidden", &flags->isModelHidden);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Collide Off", &flags->isCollideOff);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Unknown 7", &flags->field_7);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Material Code Valid", &flags->isMaterialCodeValid);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Puddle Material Valid", &flags->isPuddleMaterialValid);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Area Target On", &flags->isAreaTargetOn);

    ImGui::TableNextColumn();
    ImGui::Checkbox("Update Move FX Sensor", &flags->isUpdateMovementEffectAudioCollisionSensor);
}

void WindowActorBrowse2::displayActorInspectorTreeNrv(al::NerveKeeper* nrvKeep) {
    if (!nrvKeep)
        return;
    if (!ImGui::TreeNode("Nerves"))
        return;
    defer {
        ImGui::TreePop();
    };

    auto nrv = nrvKeep->getCurrentNerve();
    if (!nrv)
        return;

    const char* nrvName = getClassName(nrv) + strlen("(anonymous namespace)::");
    ImGui::Text("Nerve: %s", nrvName);

    if (!nrvKeep->mStateCtrl)
        return;

    auto state = nrvKeep->mStateCtrl->mCurrentState;
    if (!state)
        return;

    const char* stateName = getClassName(state);
    s32 stateNameLen = strlen(stateName);
    ImGui::Text("State: %s", stateName);

    auto stateNrvKeep = state->state->getNerveKeeper();
    if (!stateNrvKeep)
        return;

    auto stateNrv = stateNrvKeep->getCurrentNerve();
    if (!stateNrv)
        return;

    const char* stateNrvName = getClassName(stateNrvKeep) + stateNameLen + strlen("nrv");
    ImGui::Text("State Nerve: %s", stateNrvName);
}

void WindowActorBrowse2::displayActorInspectorTreeSensor(al::HitSensorKeeper* sensorKeep) {
    if (!sensorKeep || !isInStageScene())
        return;

    // TODO: Implement proper sensor viewer
    mParent->getPrimitiveQueue()->pushHitSensor(mSelectedActor, HitSensorType_ALL, 0.4f);
}
