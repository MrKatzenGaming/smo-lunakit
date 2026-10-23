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
    ImGui::Text("Type: %s", sensorTypeNames[(s32)mSensor->mSensorType]);
    ImGui::Spacing();

    ImGui::Text("Position: %.2f, %.2f, %.2f", mSensor->getPos().x, mSensor->getPos().y, mSensor->getPos().z);
    ImGui::Text("Radius: %.2f", mSensor->getRadius());
    ImGui::Spacing();

    if (mSensor->mFollowPos) {
        ImGui::Text("Follow Position: %.2f, %.2f, %.2f", mSensor->mFollowPos->x, mSensor->mFollowPos->y, mSensor->mFollowPos->z);
        ImGui::Text("Follow Offset: %.2f, %.2f, %.2f", mSensor->mFollowPosOffset.x, mSensor->mFollowPosOffset.y, mSensor->mFollowPosOffset.z);
        ImGui::Spacing();
    }

    auto mario = tryGetPlayerActorHakoniwa();
    if (mario && mSensor->getParentActor() != mario && ImGui::TreeNode("Mario Comparison")) {
        displaySensorComparison("Foot", mario->getHitSensorKeeper()->getSensor("Foot"), mSensor);

        ImGui::TreePop();
    }

    auto cappy = mario ? mario->getPlayerHackKeeper()->getHackCap() : nullptr;
    if (cappy && mSensor->getParentActor() != cappy && ImGui::TreeNode("Cappy Comparison")) {
        displaySensorComparison("Attack", cappy->getHitSensorKeeper()->getSensor("Attack"), mSensor);
        displaySensorComparison("Homing", cappy->getHitSensorKeeper()->getSensor("Eye"), mSensor);

        ImGui::TreePop();
    }

    if (mSensor->mSensorCount > 0 && ImGui::TreeNode("Collisions")) {
        for (s32 idx = 0; idx < mSensor->mSensorCount; idx++) {
            auto collidingSensor = mSensor->mSensors[idx];
            f32 depth = -(al::calcDistance(mSensor, collidingSensor) - mSensor->getRadius() - collidingSensor->getRadius());

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
void InfoProviderSensor::renderImpl() {
    getPrimitiveQueue()->pushHitSensor(mSensor, 0.8f);
}

void InfoProviderSensor::getNameImpl(sead::BufferedSafeString* buffer) const {
    buffer->format("Sensor ('%s' of '%s')", mSensor->getName(), demangleTypeName(typeid(*mSensor->mParentActor).name()));
}
