#include "hk/types.h"

#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Library/LiveActor/LiveActorFlag.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Library/Model/ModelKeeper.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateCtrl.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/Player/HackCap.h"
#include "game/Player/PlayerHackKeeper.h"

#include "devgui/windows/ActorBrowse2/ActorBrowseInspector.h"
#include "devgui/windows/WindowActorBrowse2.h"
#include "helpers/GetHelper.h"
#include "helpers/ImGuiHelper.h"
#include "imgui.h"

void ActorBrowseInspectorBase::display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) {
    auto poseKeep = actor->getPoseKeeper();
    auto flags = actor->getFlags();
    auto nrvKeep = actor->getNerveKeeper();
    auto sensorKeep = actor->getHitSensorKeeper();

    if (ImGui::TreeNodeEx("Name", cTreeFlags)) {
        displayName(actor);
        ImGui::TreePop();
    }
    if (poseKeep && ImGui::TreeNodeEx("Pose", cTreeFlags)) {
        displayPose(poseKeep);
        ImGui::TreePop();
    }
    if (flags && ImGui::TreeNodeEx("Flags", cTreeFlags)) {
        displayFlags(flags);
        ImGui::TreePop();
    }
    if (nrvKeep && ImGui::TreeNodeEx("Nerves", cTreeFlags)) {
        displayNerves(nrvKeep, strlen(getClassName(actor)) + strlen("Nrv"));
        ImGui::TreePop();
    }
    if (sensorKeep && ImGui::TreeNodeEx("Sensors", cTreeFlags)) {
        displaySensors(window, sensorKeep);
        ImGui::TreePop();
    }
}

void ActorBrowseInspectorBase::displayName(al::LiveActor* actor) {
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
}

void ActorBrowseInspectorBase::displayPose(al::ActorPoseKeeperBase* poseKeep) {
    ImGuiHelper::Vector3Drag("Translation", "Pose Keeper Translation", poseKeep->getTransPtr(), 50.f, 0.f);
    ImGuiHelper::Vector3Drag("Scale", "Pose Keeper Scale", poseKeep->getScalePtr(), 0.05f, 0.f);
    ImGuiHelper::Vector3Drag("Velocity", "Pose Keeper Velocity", poseKeep->getVelocityPtr(), 1.f, 0.f);
    ImGuiHelper::Vector3Slide("Front", "Pose Keeper Front", poseKeep->getFrontPtr(), 1.f, true);
    ImGuiHelper::Vector3Slide("Up", "Pose Keeper Up", poseKeep->getUpPtr(), 1.f, true);
    ImGuiHelper::Vector3Slide("Gravity", "Pose Keeper Gravity", poseKeep->getGravityPtr(), 1.f, true);
    ImGuiHelper::Vector3Drag("Rotation", "Pose Keeper Rotation", poseKeep->getRotatePtr(), 1.f, 360.f);
    ImGuiHelper::QuatDrag("Quaternion", "Pose Keeper Quaternion", poseKeep->getQuatPtr());
}

void ActorBrowseInspectorBase::displayFlags(al::LiveActorFlag* flags) {
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

void ActorBrowseInspectorBase::displayNerves(al::NerveKeeper* nrvKeep, s32 prefixLen) {
    auto nrv = nrvKeep->getCurrentNerve();
    if (!nrv)
        return;

    const char* nrvName = getClassName(nrv) + strlen("(anonymous namespace)::") + prefixLen;
    ImGui::Text("Active Nerve: %s", nrvName);

    if (!nrvKeep->mStateCtrl)
        return;

    auto currState = nrvKeep->mStateCtrl->mCurrentState;
    if (currState)
        ImGui::Text("Active State: %s", getClassName(currState->state));

    ImGui::Separator();

    for (s32 idx = 0; idx < nrvKeep->mStateCtrl->mStateCount; idx++) {
        auto state = &nrvKeep->mStateCtrl->mStates[idx];
        auto stateNrvKeep = state->state->getNerveKeeper();

        ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DrawLinesFull;
        if (!stateNrvKeep)
            treeFlags |= ImGuiTreeNodeFlags_Leaf;

        const char* stateName = getClassName(state->state);

        if (ImGui::TreeNodeEx(state, treeFlags, "%s (%s)", stateName, state->name)) {
            if (stateNrvKeep)
                displayNerves(stateNrvKeep, strlen(stateName) + strlen("Nrv"));

            ImGui::TreePop();
        }
    }
}

static const char* sensorTypeNames[] = {
    "Eye",
    "Player",
    "PlayerAttack",
    "PlayerFoot",
    "PlayerDecoration",
    "PlayerEye",
    "Npc",
    "Ride",
    "Enemy",
    "EnemyBody",
    "EnemyAttack",
    "EnemySimple",
    "MapObj",
    "MapObjSimple",
    "Bindable",
    "CollisionParts",
    "PlayerFireBall",
    "HoldObj",
    "LookAt",
    "BindableGoal",
    "BindableAllPlayer",
    "BindableBubbleOutScreen",
    "BindableKoura",
    "BindableRouteDokan",
    "BindableBubblePadInput",
};

static void displaySensorComparison(const char* label, al::HitSensor* sensorA, al::HitSensor* sensorB) {
    if (!sensorA || !sensorB)
        return;

    sead::Vector3f posDelta = sensorB->getPos() - sensorA->getPos();

    f32 dist = posDelta.length() - sensorA->getRadius() - sensorB->getRadius();
    f32 angle = al::wrapAngle(-sead::Mathf::atan2(posDelta.z, posDelta.x));

    ImGui::Text("%s: %.2f (%.2f°)", label, dist, angle);
}

void ActorBrowseInspectorBase::displaySensors(WindowActorBrowse2* window, al::HitSensorKeeper* sensorKeep) {
    ImGui::Checkbox("Render all Sensors of selected Actor", &window->mShouldRenderAllSensors);

    for (s32 idx = 0; idx < sensorKeep->getSensorNum(); idx++)
        displaySensor(window, sensorKeep->getSensor(idx), nullptr);
}

void ActorBrowseInspectorBase::displaySensor(WindowActorBrowse2* window, al::HitSensor* sensor, al::HitSensor* collidingSensor) {
    ImVec2 nodeMin = ImGui::GetCursorScreenPos();

    bool openNode = collidingSensor ?
                        ImGui::TreeNodeEx(sensor, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DrawLinesFull, "%s (Depth: %.2f)", sensor->getName(),
                                          -(al::calcDistance(sensor, collidingSensor) - sensor->getRadius() - collidingSensor->getRadius())) :
                        ImGui::TreeNodeEx(sensor, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DrawLinesFull, "%s", sensor->getName());

    if (openNode) {
        s32 renderIdx = window->mRenderedSensors.indexOf(sensor);
        bool shouldRender = renderIdx >= 0;
        ImGui::Checkbox("Always Render", &shouldRender);
        if (shouldRender && renderIdx < 0)
            window->mRenderedSensors.pushBack(sensor);
        else if (!shouldRender && renderIdx >= 0)
            window->mRenderedSensors.erase(renderIdx);

        ImGui::SameLine();
        if (sensor->getParentActor() && window->mSelectedActor != sensor->getParentActor()) {
            if (ImGui::Button("Select parent Actor"))
                window->mSelectedActor = sensor->getParentActor();

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
                ImGui::BeginTooltip();
                displayName(sensor->getParentActor());
                ImGui::EndTooltip();
            }
        }

        ImGui::Text("Type: %s", sensorTypeNames[(s32)sensor->mSensorType]);
        ImGui::Spacing();

        ImGui::Text("Position: %.2f, %.2f, %.2f", sensor->getPos().x, sensor->getPos().y, sensor->getPos().z);
        ImGui::Text("Radius: %.2f", sensor->getRadius());
        ImGui::Spacing();

        if (sensor->mFollowPos) {
            ImGui::Text("Follow Position: %.2f, %.2f, %.2f", sensor->mFollowPos->x, sensor->mFollowPos->y, sensor->mFollowPos->z);
            ImGui::Text("Follow Offset: %.2f, %.2f, %.2f", sensor->mFollowPosOffset.x, sensor->mFollowPosOffset.y, sensor->mFollowPosOffset.z);
            ImGui::Spacing();
        }

        auto mario = tryGetPlayerActorHakoniwa();
        if (mario && sensor->getParentActor() != mario && ImGui::TreeNode("Mario Comparison")) {
            displaySensorComparison("Foot", mario->getHitSensorKeeper()->getSensor("Foot"), sensor);

            ImGui::TreePop();
        }

        auto cappy = mario ? mario->getPlayerHackKeeper()->getHackCap() : nullptr;
        if (cappy && sensor->getParentActor() != cappy && ImGui::TreeNode("Cappy Comparison")) {
            displaySensorComparison("Attack", cappy->getHitSensorKeeper()->getSensor("Attack"), sensor);
            displaySensorComparison("Homing", cappy->getHitSensorKeeper()->getSensor("Eye"), sensor);

            ImGui::TreePop();
        }

        // Avoid infinite recursion
        if (collidingSensor == nullptr && sensor->mSensorCount > 0 && ImGui::TreeNode("Collisions")) {
            for (s32 idx = 0; idx < sensor->mSensorCount; idx++)
                displaySensor(window, sensor->mSensors[idx], sensor);

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    ImVec2 nodeMax = ImGui::GetCursorScreenPos();
    nodeMax.x += ImGui::GetContentRegionAvail().x;  // Span full width

    if (!window->mHoveredSensor && ImGui::IsMouseHoveringRect(nodeMin, nodeMax))
        window->mHoveredSensor = sensor;
}
