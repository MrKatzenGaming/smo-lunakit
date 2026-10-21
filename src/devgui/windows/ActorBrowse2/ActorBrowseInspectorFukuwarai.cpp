
#include "al/Library/Base/StringUtil.h"
#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Library/LiveActor/ActorModelFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/LiveActor/LiveActorGroup.h"
#include "al/Library/LiveActor/LiveActorKit.h"
#include "al/Library/Math/MathUtil.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/Npc/FukuwaraiFaceParts.h"
#include "game/Npc/FukuwaraiWatcher.h"
#include "game/Player/PlayerHackKeeper.h"

#include "devgui/windows/ActorBrowse2/ActorBrowseInspector.h"
#include "devgui/windows/WindowActorBrowse2.h"
#include "imgui.h"
#include "imgui_internal.h"

FukuwaraiPart KuriboEyebrowLeftPart = {"FukuwaraiKuriboEyebrowLeft", 4.0f, 12.0f, 4.0f};
FukuwaraiPart KuriboEyebrowRightPart = {"FukuwaraiKuriboEyebrowRight", 4.0f, 12.0f, 4.0f};
FukuwaraiPart KuriboEyeLeftPart = {"FukuwaraiKuriboEyeLeft", 4.0f, 12.0f, 4.0f};
FukuwaraiPart KuriboEyeRightPart = {"FukuwaraiKuriboEyeRight", 4.0f, 12.0f, 4.0f};
FukuwaraiPart KuriboMouthAngryPart = {"FukuwaraiKuriboMouthAngry", 4.0f, 12.0f, 4.0f};

FukuwaraiPart MarioEyebrowLeftPart = {"FukuwaraiMarioEyebrowLeft", 3.0f, 9.0f, 3.0f};
FukuwaraiPart MarioEyebrowRightPart = {"FukuwaraiMarioEyebrowRight", 3.0f, 9.0f, 3.0f};
FukuwaraiPart MarioEyeLeftPart = {"FukuwaraiMarioEyeLeft", 3.0f, 9.0f, 3.0f};
FukuwaraiPart MarioEyeRightPart = {"FukuwaraiMarioEyeRight", 3.0f, 9.0f, 3.0f};
FukuwaraiPart MarioMouthPart = {"FukuwaraiMarioMouth", 3.0f, 9.0f, 3.0f};
FukuwaraiPart MarioMustachePart = {"FukuwaraiMarioMustache", 3.0f, 9.0f, 3.0f};
FukuwaraiPart MarioNosePart = {"FukuwaraiMarioNose", 2.0f, 6.0f, 2.0f};

static void displayParts(FukuwaraiFaceParts* parts) {
    const char* name = al::getModelName(parts);
    FukuwaraiPart* part = nullptr;

    // Mario
    if (al::isEqualString(name, MarioEyebrowLeftPart.name))
        part = &MarioEyebrowLeftPart;
    else if (al::isEqualString(name, MarioEyebrowRightPart.name))
        part = &MarioEyebrowRightPart;
    else if (al::isEqualString(name, MarioEyeLeftPart.name))
        part = &MarioEyeLeftPart;
    else if (al::isEqualString(name, MarioEyeRightPart.name))
        part = &MarioEyeRightPart;
    else if (al::isEqualString(name, MarioMouthPart.name))
        part = &MarioMouthPart;
    else if (al::isEqualString(name, MarioMustachePart.name))
        part = &MarioMustachePart;
    else if (al::isEqualString(name, MarioNosePart.name))
        part = &MarioNosePart;
    // Kuribo
    else if (al::isEqualString(name, KuriboEyebrowLeftPart.name))
        part = &KuriboEyebrowLeftPart;
    else if (al::isEqualString(name, KuriboEyebrowRightPart.name))
        part = &KuriboEyebrowRightPart;
    else if (al::isEqualString(name, KuriboEyeLeftPart.name))
        part = &KuriboEyeLeftPart;
    else if (al::isEqualString(name, KuriboEyeRightPart.name))
        part = &KuriboEyeRightPart;
    else if (al::isEqualString(name, KuriboMouthAngryPart.name))
        part = &KuriboMouthAngryPart;

    if (part == nullptr)
        return;

    ImGui::Text("Base Points: %.0f", part->basePoints);
    ImGui::Text("Distance Points: %.0f", part->distancePoints);
    ImGui::Text("Angle Points: %.0f", part->anglePoints);
    ImGui::Separator();

    f32 distRate = parts->calcScoreDistRate();
    f32 angleRate = parts->calcScoreAngleRate();

    {
        sead::Vector3f pos = al::getTrans(parts);
        sead::Vector3f difference;
        al::verticalizeVec(&difference, sead::Vector3f::ey, pos - parts->mTargetPos);

        f32 score = difference.length() / 500.0f;
        score = sead::Mathf::clamp(score, 0.0f, 1.0f);

        ImGui::Text("Current Position: %.3f, %.3f, %.3f", pos.x, pos.y, pos.z);
        ImGui::Text("Target Position: %.3f, %.3f, %.3f", parts->mTargetPos.x, parts->mTargetPos.y, parts->mTargetPos.z);
        ImGui::Text("Position Difference: %.3f, %.3f, %.3f (%.3f)", difference.x, difference.y, difference.z, difference.length());
        ImGui::Spacing();
        ImGui::Text("Distance Rate: %.3f", distRate);
        ImGui::Text("Distance Score: %.3f", part->distancePoints * distRate);
        ImGui::Separator();
    }

    {
        f32 angle = al::getRotate(parts).y;

        f32 difference;
        if (angle > parts->mTargetAngle)
            difference = angle - parts->mTargetAngle;
        else
            difference = parts->mTargetAngle - angle;

        difference = difference > 180.0f ? 360.0f - difference : difference;

        if (al::isEqualString("FukuwaraiMarioNose", al::getModelName(parts)))
            difference = difference > 90.0f ? 180.0f - difference : difference;

        ImGui::Text("Current Rotation: %.3f", al::getRotate(parts).y);
        ImGui::Text("Target Rotation: %.3f", parts->mTargetAngle);
        ImGui::Text("Rotation Difference: %.3f", difference);
        ImGui::Spacing();
        ImGui::Text("Angle Rate: %.3f", angleRate);
        ImGui::Text("Angle Score: %.3f", part->anglePoints * angleRate);
        ImGui::Separator();
    }

    ImGui::Text("Score: %.3f", part->basePoints + part->distancePoints * distRate + part->anglePoints * angleRate);
}

void ActorBrowseInspectorFukuwaraiFaceParts::display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) {
    ActorBrowseInspectorBase::display(window, scene, actor);

    FukuwaraiFaceParts* parts = reinterpret_cast<FukuwaraiFaceParts*>(actor);
    // FukuwaraiWatcher* watcher = nullptr;

    // auto allActorGroup = scene->getLiveActorKit()->getLiveActorGroupAllActors();
    // for (s32 idx = 0; idx < allActorGroup->getActorCount(); idx++) {
    //     auto actor = allActorGroup->getActor(idx);
    //     if (al::isEqualString("FukuwaraiWatcher", actor->getName())) {
    //         watcher = reinterpret_cast<FukuwaraiWatcher*>(actor);
    //         break;
    //     }
    // }

    // if (watcher) {
    //     ImGui::Text("Total Score: %d", watcher->calcTotalScore());
    //     ImGui::Separator();
    // }

    displayParts(parts);
}

void ActorBrowseInspectorFukuwaraiWatcher::display(WindowActorBrowse2* window, al::Scene* scene, al::LiveActor* actor) {
    ActorBrowseInspectorBase::display(window, scene, actor);

    auto watcher = reinterpret_cast<FukuwaraiWatcher*>(actor);

    ImGui::Text("Total Score: %d", watcher->calcTotalScore());

    ImGui::SeparatorText("Placed Parts");
    for (s32 idx = 0; idx < watcher->mPlacedParts.size(); idx++) {
        auto parts = watcher->mPlacedParts.at(idx);

        if (ImGui::TreeNode(parts, "%s", parts->getName())) {
            displayParts(parts);
            ImGui::TreePop();
        }
    }
}
