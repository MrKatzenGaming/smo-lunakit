#include "info/InfoProviderFukuwarai.h"

#include "sead/math/seadVector.h"

#include "al/Library/LiveActor/ActorModelFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/Math/MathUtil.h"

#include "game/MapObj/GrowSeedStateWait.h"
#include "game/Npc/FukuwaraiFaceParts.h"
#include "game/Npc/FukuwaraiNpc.h"
#include "game/Npc/FukuwaraiWatcher.h"

#include "helpers/NrvHelper.h"
#include "imgui.h"
#include "info/InfoProviderActor.h"
#include "info/InfoProviderEventFlow.h"

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

void InfoProviderFukuwaraiFaceParts::displayImpl() {
    InfoProviderActor::displayImpl();

    auto parts = getKey<FukuwaraiFaceParts>();

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
void InfoProviderFukuwaraiFaceParts::renderImpl(RenderInfo info) {
    InfoProviderActor::renderImpl(info);

    auto parts = getKey<FukuwaraiFaceParts>();

    constexpr f32 rotationLineLength = 200.0f;
    f32 currAngle = sead::Mathf::deg2rad(al::getRotate(parts).y);
    f32 targetAngle = sead::Mathf::deg2rad(parts->mTargetAngle);
    auto& trans = al::getTrans(parts);

    if (info.renderSelf) {
        auto primQueue = getPrimitiveQueue();
        primQueue->pushPoint(parts->mTargetPos, 15.0f, sead::Color4f(0.0f, 1.0f, 0.0f, 0.5f));
        primQueue->pushLine(
            trans, trans + sead::Vector3f(sead::Mathf::cos(currAngle) * rotationLineLength, 0.0f, sead::Mathf::sin(currAngle) * rotationLineLength),
            sead::Color4f(1.0f, 0.0f, 1.0f, 0.75f));
        primQueue->pushLine(parts->mTargetPos,
                            parts->mTargetPos + sead::Vector3f(sead::Mathf::cos(targetAngle) * rotationLineLength, 0.0f,
                                                               sead::Mathf::sin(targetAngle) * rotationLineLength),
                            sead::Color4f(1.0f, 0.0f, 0.0f, 0.75f));
    }
}

void InfoProviderFukuwaraiNpc::displayImpl() {
    InfoProviderActor::displayImpl();

    auto npc = getKey<FukuwaraiNpc>();
    ImGui::Text("Score: %d", npc->mScore);
    ImGui::Text("Success Score: %d", npc->mSuccessScore);

    if (ImGui::TreeNodeEx("Event Flow", cFramedNode)) {
        display<InfoProviderEventFlow>(npc->mEventFlow);
        ImGui::TreePop();
    }
}
void InfoProviderFukuwaraiNpc::renderImpl(RenderInfo info) {
    InfoProviderActor::renderImpl(info);

    auto npc = getKey<FukuwaraiNpc>();
    render<InfoProviderEventFlow>(info, npc->mEventFlow);
}

void InfoProviderFukuwaraiWatcher::displayImpl() {
    InfoProviderActor::displayImpl();

    auto watcher = getKey<FukuwaraiWatcher>();
    ImGui::Text("Total Score: %d", watcher->calcTotalScore());

    ImGui::SeparatorText("Placed Parts");
    for (s32 idx = 0; idx < watcher->mPlacedParts.size(); idx++) {
        auto parts = watcher->mPlacedParts.at(idx);
        ImGui::BulletText("%s: %.3f", parts->getName(), parts->calcScore(watcher->mIsMario));
    }
}
