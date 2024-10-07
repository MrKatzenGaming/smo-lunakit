#include "program/devgui/categories/info/CategoryInfCappy.h"

#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveKeeper.h"
#include "Library/Nerve/NerveStateCtrl.h"

#include "game/Player/PlayerActorBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerFunction.h"
#include "game/Player/PlayerAnimator.h"
#include "game/Player/PlayerRecoverySafetyPoint.h"

#include "rs/util.hpp"
#include "helpers/GetHelper.h"

#include "imgui.h"

#include <cxxabi.h>
#include <helpers/ImGuiHelper.h>
#include <Library/LiveActor/ActorMovementFunction.h>
#include "Library/LiveActor/ActorPoseKeeper.h"
#include <gfx/seadCamera.h>
#include "al/util/GraphicsUtil.h"
#include "logger/Logger.hpp"

CategoryInfCappy::CategoryInfCappy(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap) {}

void CategoryInfCappy::updateCatDisplay()
{
    PlayerActorBase* player = tryGetPlayerActor();


    if(!player) {
        ImGui::Text("Player does not exist!");
        return;
    }
    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    al::LiveActor* cappy = playerHak->mHackCap;

    /*
        Precision Slider
    */

    ImGui::SliderInt("Precision", &sliderValue, 0, 10);
    snprintf(format, sizeof(format), "%%.%df", static_cast<int>(sliderValue));

    /*
        // PLAYER CLASS, STATE, AND NERVES
    */
    
    // Actor name and nerve

    char* stateName = nullptr;
    char* stateNrvName = nullptr;

    int status;
    const al::Nerve* cappyNerve = cappy->getNerveKeeper()->getCurrentNerve();
    
    if (cappy->getNerveKeeper()->mStateCtrl) {
        al::NerveStateCtrl::State* state = cappy->getNerveKeeper()->mStateCtrl->findStateInfo(cappyNerve);
        if(state) {
            const al::Nerve* stateNerve = state->state->getNerveKeeper()->getCurrentNerve();
            stateName = abi::__cxa_demangle(typeid(*state->state).name(), nullptr, nullptr, &status);
            stateNrvName = abi::__cxa_demangle(typeid(*stateNerve).name(), nullptr, nullptr, &status);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////

    al::ActorPoseKeeperBase* pose = cappy->mPoseKeeper;
    StageScene* stageScene = tryGetStageScene();

    if(!pose)
        return;

    float hSpeed = al::calcSpeedH(cappy), vSpeed = al::calcSpeedV(cappy), speed = al::calcSpeed(cappy);
    float hSpeedAngle = atan2f(pose->getVelocityPtr()->z, pose->getVelocityPtr()->x);
    if (hSpeedAngle < 0)
        hSpeedAngle += M_PI * 2;
        
    float hSpeedAngleDeg = DEG(hSpeedAngle);

    static sead::Vector3f prevCappyVel = { 0.0f, 0.0f, 0.0f };
    sead::Vector3f cappyVelDelta = pose->getVelocity() - prevCappyVel;

    ImGui::DragFloat3("Trans", &pose->mTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Velocity", &pose->getVelocityPtr()->x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Vel Delta", &cappyVelDelta.x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);

    ImGui::DragFloat("Vel Angle", &hSpeedAngleDeg, 1.f, 0.f, 360.f, format, ImGuiSliderFlags_NoInput);
    prevCappyVel = pose->getVelocity();
    ImGuiHelper::Quat("Player Quaternion", pose->getQuatPtr());
    
    if (pose->getQuatPtr() != nullptr){
        f32 x = pose->getQuatPtr()->z;
        f32 y = pose->getQuatPtr()->y;
        f32 z = pose->getQuatPtr()->x;
        f32 w = pose->getQuatPtr()->w;

        // Compute Euler angles
        f32 t0 = 2.0f * (w * x + y * z);
        f32 t1 = 1.0f - 2.0f * (x * x + y * y);
        f32 roll = atan2f(t0, t1);

        f32 t2 = 2.0f * (w * y - z * x);
        t2 = t2 > 1.0f ? 1.0f : t2;
        t2 = t2 < -1.0f ? -1.0f : t2;
        f32 pitch = asinf(t2);

        f32 t3 = 2.0f * (w * z + x * y);
        f32 t4 = 1.0f - 2.0f * (y * y + z * z);
        f32 yaw = atan2f(t3, t4);

        sead::Vector3f cappyEulerAngles = {yaw, pitch, roll};

        sead::Vector3f cappyRot = sead::Vector3f(DEG(cappyEulerAngles.x), DEG(cappyEulerAngles.y), DEG(cappyEulerAngles.z));
        ImGui::DragFloat3("Euler", &cappyRot.x, 1.f, -1.f, 1.f, format, ImGuiSliderFlags_NoRoundToFormat);
    }

    if(stateName && stateNrvName) {
        ImGui::Text("State: %s", stateName);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Current type of action\nthe player is performing");

        ImGui::Text("State Nrv: %s", stateNrvName + 23 + strlen(stateName) + 3);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Basically the sub-action\nCurrent nerve of the state");

        free(stateName);
        free(stateNrvName);
    }

}