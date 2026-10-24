#include "info/InfoProviderActor.h"

#include "hk/types.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Library/LiveActor/LiveActorFlag.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Library/Model/ModelKeeper.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/Player/PlayerHackKeeper.h"

#include "helpers/ImGuiHelper.h"
#include "imgui.h"
#include "info/InfoProviderFukuwarai.h"
#include "info/InfoProviderGrowSeed.h"
#include "info/InfoProviderNerve.h"
#include "info/InfoProviderSensor.h"

void InfoProviderActor::displayImpl() {
    auto poseKeep = mActor->getPoseKeeper();
    auto flags = mActor->getFlags();
    auto nrvKeep = mActor->getNerveKeeper();
    auto sensorKeep = mActor->getHitSensorKeeper();

    if (ImGui::TreeNodeEx("Name", cFramedNode)) {
        displayName();
        ImGui::TreePop();
    }
    if (poseKeep && ImGui::TreeNodeEx("Pose", cFramedNode)) {
        displayPose();
        ImGui::TreePop();
    }
    if (flags && ImGui::TreeNodeEx("Flags", cFramedNode)) {
        displayFlags();
        ImGui::TreePop();
    }
    if (nrvKeep && ImGui::TreeNodeEx("Nerves", cFramedNode)) {
        InfoProviderNerveKeeper(nrvKeep).display();
        ImGui::TreePop();
    }
    if (sensorKeep && ImGui::TreeNodeEx("Sensors", cFramedNode)) {
        displaySensors();
        ImGui::TreePop();
    }
}
void InfoProviderActor::renderImpl() {
    auto nrvKeep = mActor->getNerveKeeper();

    renderPose();
    if (nrvKeep)
        InfoProviderNerveKeeper(nrvKeep).render();
    renderSensors();
}

void InfoProviderActor::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("Actor ('%s')", demangleTypeName(typeid(*mActor).name()));
}

void InfoProviderActor::displayActor(al::LiveActor* actor) {
    const char* name = typeid(*actor).name();

    if (al::isEqualString(name, InfoProviderGrowPlantSeed::cClassName)) {
        InfoProviderGrowPlantSeed(actor).display();
    } else if (al::isEqualString(name, InfoProviderFukuwaraiFaceParts::cClassName)) {
        InfoProviderFukuwaraiFaceParts(actor).display();
    } else if (al::isEqualString(name, InfoProviderFukuwaraiNpc::cClassName)) {
        InfoProviderFukuwaraiNpc(actor).display();
    } else if (al::isEqualString(name, InfoProviderFukuwaraiWatcher::cClassName)) {
        InfoProviderFukuwaraiWatcher(actor).display();
    } else {
        InfoProviderActor(actor).display();
    }
}
void InfoProviderActor::renderActor(al::LiveActor* actor) {
    const char* name = typeid(*actor).name();

    if (al::isEqualString(name, InfoProviderGrowPlantSeed::cClassName)) {
        InfoProviderGrowPlantSeed(actor).render();
    } else if (al::isEqualString(name, InfoProviderFukuwaraiFaceParts::cClassName)) {
        InfoProviderFukuwaraiFaceParts(actor).render();
    } else if (al::isEqualString(name, InfoProviderFukuwaraiNpc::cClassName)) {
        InfoProviderFukuwaraiNpc(actor).render();
    } else {
        InfoProviderActor(actor).render();
    }
}

void InfoProviderActor::displayName() {
    auto model = mActor->getModelKeeper();

    const char* className = demangleTypeName(typeid(*mActor).name());
    const char* actorName = mActor->getName();
    const char* modelName = model ? model->getName() : nullptr;

    if (className)
        ImGui::Text("Class: %s", className);
    if (actorName)
        ImGui::Text("Actor: %s", actorName);
    if (modelName)
        ImGui::Text("Model: %s", modelName);
}

void InfoProviderActor::displayPose() {
    auto poseKeep = mActor->getPoseKeeper();

    ImGuiHelper::Vector3Drag("Translation", "Pose Keeper Translation", poseKeep->getTransPtr(), 50.f, 0.f);
    ImGuiHelper::Vector3Drag("Scale", "Pose Keeper Scale", poseKeep->getScalePtr(), 0.05f, 0.f);
    ImGuiHelper::Vector3Drag("Velocity", "Pose Keeper Velocity", poseKeep->getVelocityPtr(), 1.f, 0.f);
    ImGuiHelper::Vector3Slide("Front", "Pose Keeper Front", poseKeep->getFrontPtr(), 1.f, true);
    ImGuiHelper::Vector3Slide("Up", "Pose Keeper Up", poseKeep->getUpPtr(), 1.f, true);
    ImGuiHelper::Vector3Slide("Gravity", "Pose Keeper Gravity", poseKeep->getGravityPtr(), 1.f, true);
    ImGuiHelper::Vector3Drag("Rotation", "Pose Keeper Rotation", poseKeep->getRotatePtr(), 1.f, 360.f);
    ImGuiHelper::QuatDrag("Quaternion", "Pose Keeper Quaternion", poseKeep->getQuatPtr());
}
void InfoProviderActor::renderPose() {
    auto poseKeep = mActor->getPoseKeeper();
    if (!poseKeep)
        return;

    getPrimitiveQueue()->pushPoint(poseKeep->getTrans(), 10.0f, sead::Color4f(0.25f, 0.5f, 0.5f, 0.75f));
}

void InfoProviderActor::displayFlags() {
    auto flags = mActor->getFlags();

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

void InfoProviderActor::displaySensors() {
    auto sensorKeep = mActor->getHitSensorKeeper();

    for (s32 idx = 0; idx < sensorKeep->getSensorNum(); idx++) {
        auto sensor = sensorKeep->getSensor(idx);
        if (ImGui::TreeNodeEx(sensor, cFramedNode, "%s", sensor->getName())) {
            InfoProviderSensor(sensor).display();
            ImGui::TreePop();
        }
    }
}
void InfoProviderActor::renderSensors() {
    auto sensorKeep = mActor->getHitSensorKeeper();
    if (!sensorKeep)
        return;

    for (s32 idx = 0; idx < sensorKeep->getSensorNum(); idx++) {
        auto sensor = sensorKeep->getSensor(idx);
        InfoProviderSensor(sensor).render();
    }
}
