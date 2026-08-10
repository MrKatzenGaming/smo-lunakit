#include "devgui/categories/info/CategoryInfSensors.h"

#include "sead/math/seadVectorFwd.h"

#include "al/Library/HitSensor/HitSensorKeeper.h"
#include "al/Library/LiveActor/ActorMovementFunction.h"
#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Project/HitSensor/HitSensor.h"

#include "game/Player/PlayerActorHakoniwa.h"

#include "custom/game/Player/HackCap.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "helpers/GetHelper.h"
#include "imgui.h"
#define DEG(X) X * 180 / M_PI

CategoryInfSensors::CategoryInfSensors(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryInfSensors::updateCatDisplay() {
    PlayerActorHakoniwa* p = tryGetPlayerActorHakoniwa();
    if (!p) {
        ImGui::Text("Player does not exist!");
        return;
    }

    ImGui::SliderInt("Precision", &sliderValue, 0, 10);
    snprintf(format, sizeof(format), "%%.%df", static_cast<int>(sliderValue));

    /* ----------Mario----------
        0 Foot
        1 Body
        2 Head
        18 SpinAttack
        19 Carry
        20 Eye
    */
    al::HitSensor* marioFoot = p->getHitSensorKeeper()->getSensor(0);
    al::HitSensor* marioBody = p->getHitSensorKeeper()->getSensor(1);
    al::HitSensor* marioHead = p->getHitSensorKeeper()->getSensor(2);
    al::HitSensor* marioSpinAttack = p->getHitSensorKeeper()->getSensor(18);
    al::HitSensor* marioCarry = p->getHitSensorKeeper()->getSensor(19);
    al::HitSensor* marioEye = p->getHitSensorKeeper()->getSensor(20);
    if (ImGui::CollapsingHeader("Mario Sensors")) {
        ImGui::Indent();
        ImGui::DragFloat3("Foot", &marioFoot->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Body", &marioBody->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Head", &marioHead->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("SpinAttack", &marioSpinAttack->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Carry", &marioCarry->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Eye", &marioEye->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::Unindent();
    }

    if (!p->mHackCap) {
        ImGui::Text("Cappy does not exist!");
        return;
    }

    /* ----------Cappy----------
        0 Attack
        6 Catch
        7 Trample
        8 Eye
        9 Push
    */
    al::HitSensor* capAttack = p->mHackCap->getHitSensorKeeper()->getSensor(0);
    al::HitSensor* capCatch = p->mHackCap->getHitSensorKeeper()->getSensor(6);
    al::HitSensor* capTrample = p->mHackCap->getHitSensorKeeper()->getSensor(7);
    al::HitSensor* capEye = p->mHackCap->getHitSensorKeeper()->getSensor(8);
    al::HitSensor* capPush = p->mHackCap->getHitSensorKeeper()->getSensor(9);
    if (ImGui::CollapsingHeader("Cappy Sensors")) {
        ImGui::Indent();
        ImGui::DragFloat3("Attack", &capAttack->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Catch", &capCatch->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Trample", &capTrample->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Eye", &capEye->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Push", &capPush->mPos.x, 50, 0, 0, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::Unindent();
    }

    float distFootCatch = al::calcDistance(marioFoot, capCatch);
    sead::Vector3f dist = marioFoot->mPos - capCatch->mPos;
    float distFootCatchV = abs(dist.y);
    float thetaFootCatch = atanf(dist.y / sqrtf(pow(dist.x, 2) + pow(dist.z, 2)));

    snprintf(textBuffer, sizeof(textBuffer), "M.Foot-C.Catch V: %s", format);
    ImGui::Text(textBuffer, distFootCatchV);
    ImGui::SameLine();
    snprintf(textBuffer, sizeof(textBuffer), "S: %s", format);
    ImGui::Text(textBuffer, distFootCatch);
    ImGui::SameLine();
    snprintf(textBuffer, sizeof(textBuffer), "theta: %s", format);
    ImGui::Text(textBuffer, DEG(thetaFootCatch));

    float distBodyCap = al::calcDistance(p->mHackCap, marioBody->mPos);
    snprintf(textBuffer, sizeof(textBuffer), "M.Body-Cappy: %s", format);
    ImGui::Text(textBuffer, distBodyCap);

    float distHeadCap = al::calcDistance(p->mHackCap, marioHead->mPos);
    snprintf(textBuffer, sizeof(textBuffer), "M.Head-Cappy: %s", format);
    ImGui::Text(textBuffer, distBodyCap);
}
