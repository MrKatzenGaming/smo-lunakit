#include "info/InfoProviderSensor.h"

#include "sead/prim/seadSafeString.h"

#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/Player/HackCap.h"
#include "game/Player/PlayerHackKeeper.h"

#include <typeinfo>

#include "helpers/GetHelper.h"
#include "imgui.h"
#include "info/InfoProviderActor.h"

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

void InfoProviderSensor::displayImpl() {
    auto sensor = getKey<Key>();

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

    if (sensor->mSensorCount > 0 && ImGui::TreeNode("Collisions")) {
        for (s32 idx = 0; idx < sensor->mSensorCount; idx++) {
            auto collidingSensor = sensor->mSensors[idx];
            f32 depth = -(al::calcDistance(sensor, collidingSensor) - sensor->getRadius() - collidingSensor->getRadius());

            ImGui::BulletText("%s (Depth: %.2f)", collidingSensor->getName(), depth);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip)) {
                ImGui::BeginTooltip();

                ImGui::Text("Parent Actor:");
                ImGui::Separator();

                InfoProviderActor provider(collidingSensor->getParentActor());
                provider.displayName();

                ImGui::EndTooltip();
            }
        }

        ImGui::TreePop();
    }
}
void InfoProviderSensor::renderImpl(RenderInfo info) {
    auto sensor = getKey<Key>();
    if (info.renderSelf && al::isSensorValid(sensor))
        getPrimitiveQueue()->pushHitSensor(sensor, 0.8f);
}

void InfoProviderSensor::getNameImpl(sead::BufferedSafeString* buffer) const {
    auto sensor = getKey<Key>();
    buffer->format("Sensor ('%s' of '%s')", sensor->getName(), demangleTypeName(typeid(*sensor->mParentActor).name()));
}
