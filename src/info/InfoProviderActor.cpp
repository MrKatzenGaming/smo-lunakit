#include "info/InfoProviderActor.h"

#include "hk/types.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/Base/StringUtil.h"
#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Library/LiveActor/LiveActor.h"
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
    auto actor = getKey<Key>();

    auto poseKeep = actor->getPoseKeeper();
    auto flags = actor->getFlags();
    auto nrvKeep = actor->getNerveKeeper();
    auto sensorKeep = actor->getHitSensorKeeper();

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
        display<InfoProviderNerveKeeper>(nrvKeep);
        ImGui::TreePop();
    }
    if (sensorKeep && ImGui::TreeNodeEx("Sensors", cFramedNode)) {
        displaySensors();
        ImGui::TreePop();
    }
}
void InfoProviderActor::renderImpl(RenderInfo info) {
    auto actor = getKey<Key>();

    if (info.renderSelf)
        renderPose();

    render<InfoProviderNerveKeeper>(info, actor->getNerveKeeper());
    renderSensors(info);
}

void InfoProviderActor::getNameImpl(sead::BufferedSafeString* buffer) const {
    auto actor = getKey<Key>();
    buffer->format("Actor ('%s')", demangleTypeName(typeid(*actor).name()));
}

void InfoProviderActor::displayActor(al::LiveActor* actor) {
    const char* name = typeid(*actor).name();

    if (al::isEqualString(name, InfoProviderGrowPlantSeed::cClassName))
        display<InfoProviderGrowPlantSeed>(actor);
    else if (al::isEqualString(name, InfoProviderFukuwaraiFaceParts::cClassName))
        display<InfoProviderFukuwaraiFaceParts>(actor);
    else if (al::isEqualString(name, InfoProviderFukuwaraiNpc::cClassName))
        display<InfoProviderFukuwaraiNpc>(actor);
    else if (al::isEqualString(name, InfoProviderFukuwaraiWatcher::cClassName))
        display<InfoProviderFukuwaraiWatcher>(actor);
    else
        display<InfoProviderActor>(actor);
}
void InfoProviderActor::renderActor(RenderInfo info, al::LiveActor* actor) {
    const char* name = typeid(*actor).name();

    if (al::isEqualString(name, InfoProviderGrowPlantSeed::cClassName))
        render<InfoProviderGrowPlantSeed>(info, actor);
    else if (al::isEqualString(name, InfoProviderFukuwaraiFaceParts::cClassName))
        render<InfoProviderFukuwaraiFaceParts>(info, actor);
    else if (al::isEqualString(name, InfoProviderFukuwaraiNpc::cClassName))
        render<InfoProviderFukuwaraiNpc>(info, actor);
    else
        render<InfoProviderActor>(info, actor);
}

void InfoProviderActor::displayName() {
    auto actor = getKey<Key>();
    auto model = actor->getModelKeeper();

    const char* className = demangleTypeName(typeid(*actor).name());
    const char* actorName = actor->getName();
    const char* modelName = model ? model->getName() : nullptr;

    if (className)
        ImGui::Text("Class: %s", className);
    if (actorName)
        ImGui::Text("Actor: %s", actorName);
    if (modelName)
        ImGui::Text("Model: %s", modelName);
}

void InfoProviderActor::displayPose() {
    auto actor = getKey<Key>();
    auto poseKeep = actor->getPoseKeeper();

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
    auto actor = getKey<Key>();
    auto poseKeep = actor->getPoseKeeper();
    if (!poseKeep)
        return;

    getPrimitiveQueue()->pushPoint(poseKeep->getTrans(), 10.0f, sead::Color4f(0.25f, 0.5f, 0.5f, 0.75f));
}

void InfoProviderActor::displayFlags() {
    auto actor = getKey<Key>();
    auto flags = actor->getFlags();

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
    auto actor = getKey<Key>();
    auto sensorKeep = actor->getHitSensorKeeper();

    for (s32 idx = 0; idx < sensorKeep->getSensorNum(); idx++) {
        auto sensor = sensorKeep->getSensor(idx);
        if (ImGui::TreeNodeEx(sensor, cFramedNode, "%s", sensor->getName())) {
            display<InfoProviderSensor>(sensor);
            ImGui::TreePop();
        }
    }
}
void InfoProviderActor::renderSensors(RenderInfo info) {
    auto actor = getKey<Key>();
    auto sensorKeep = actor->getHitSensorKeeper();
    if (!sensorKeep)
        return;

    for (s32 idx = 0; idx < sensorKeep->getSensorNum(); idx++) {
        auto sensor = sensorKeep->getSensor(idx);
        render<InfoProviderSensor>(info, sensor);
    }
}
