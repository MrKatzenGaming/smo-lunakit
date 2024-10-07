#include "program/devgui/categories/info/CategoryInfPlayer.h"

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

CategoryInfPlayer::CategoryInfPlayer(const char* catName, const char* catDesc, sead::Heap* heap)
    : CategoryBase(catName, catDesc, heap) {}

void CategoryInfPlayer::updateCatDisplay()
{
    PlayerActorBase* player = tryGetPlayerActor();

    if(!player) {
        ImGui::Text("Player does not exist!");
        return;
    }

    /*
        Precision Slider
    */

    ImGui::SliderInt("Precision", &sliderValue, 0, 10);
    snprintf(format, sizeof(format), "%%.%df", static_cast<int>(sliderValue));

    /*
        // GENERIC BOOLEAN INFO
    */

    bool isDead = PlayerFunction::isPlayerDeadStatus(player);
    bool isDemo = rs::isActiveDemo(player);
    bool isGround = rs::isPlayerOnGround(player);

    ImGui::Checkbox("Dead", &isDead);
    ImGui::SameLine();
    ImGui::Checkbox("Demo", &isDemo);
    ImGui::SameLine();
    ImGui::Checkbox("Grounded", &isGround);

    /*
        // PLAYER CLASS, STATE, AND NERVES
    */
    
    // Actor name and nerve

    char* playerName = nullptr;
    char* stateName = nullptr;
    char* stateNrvName = nullptr;

    int status;
    const al::Nerve* playerNerve = player->getNerveKeeper()->getCurrentNerve();
    playerName = abi::__cxa_demangle(typeid(*player).name(), nullptr, nullptr, &status);
    
    if (player->getNerveKeeper()->mStateCtrl) {
        al::NerveStateCtrl::State* state = player->getNerveKeeper()->mStateCtrl->findStateInfo(playerNerve);
        if(state) {
            const al::Nerve* stateNerve = state->state->getNerveKeeper()->getCurrentNerve();
            stateName = abi::__cxa_demangle(typeid(*state->state).name(), nullptr, nullptr, &status);
            stateNrvName = abi::__cxa_demangle(typeid(*stateNerve).name(), nullptr, nullptr, &status);
        }
    }

    if (playerName) {
        ImGui::Text("Class: %s", playerName);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Type of Player Actor\nHakoniwa is normal Mario\nYukimaru is Shiverian Racer");
        free(playerName);
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

    /*
        // PLAYER ANIMATIONS (ONLY IF PlayerActorHakoniwa)
    */

    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    if(!playerHak) {
        ImGui::TextDisabled("Cannot display additional info, not Hakoniwa");
        return;
    }

    PlayerAnimator* anim = playerHak->mAnimator;
    ImGui::Text("Anim: %s (%.00f/%.00f)", anim->mCurAnim.cstr(), anim->getAnimFrame(), anim->getAnimFrameMax());
    ImGui::Text("Sub Anim: %s (%.00f/%.00f)", anim->mCurSubAnim.cstr(), anim->getSubAnimFrame(), anim->getSubAnimFrameMax());

    //////////////////////////////////////////////////////////////////////////////////////////////////
    if (!playerHak)
        return;

    al::ActorPoseKeeperBase* pose = playerHak->mPoseKeeper;
    StageScene* stageScene = tryGetStageScene();

    if(!pose)
        return;

    float hSpeed = al::calcSpeedH(playerHak), vSpeed = al::calcSpeedV(playerHak), speed = al::calcSpeed(playerHak);
    float hSpeedAngle = atan2f(pose->getVelocityPtr()->z, pose->getVelocityPtr()->x);
    if (hSpeedAngle < 0)
        hSpeedAngle += M_PI * 2;
        
    float hSpeedAngleDeg = DEG(hSpeedAngle);

    static sead::Vector3f prevPlayerVel = { 0.0f, 0.0f, 0.0f };
    sead::Vector3f playerVelDelta = pose->getVelocity() - prevPlayerVel;

    ImGui::DragFloat3("Trans", &pose->mTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Velocity", &pose->getVelocityPtr()->x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Vel Delta", &playerVelDelta.x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);

    ImGui::DragFloat("Vel Angle", &hSpeedAngleDeg, 1.f, 0.f, 360.f, format, ImGuiSliderFlags_NoInput);
    prevPlayerVel = pose->getVelocity();
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

        sead::Vector3f playerEulerAngles = {yaw, pitch, roll};

        sead::Vector3f playerRot = sead::Vector3f(DEG(playerEulerAngles.x), DEG(playerEulerAngles.y), DEG(playerEulerAngles.z));
        ImGui::DragFloat3("Euler", &playerRot.x, 1.f, -1.f, 1.f, format, ImGuiSliderFlags_NoRoundToFormat);
        }

    sead::LookAtCamera* camera = al::getLookAtCamera(stageScene, 0);

    sead::Vector3f cameraPos = camera->mPos;
    sead::Vector3f cameraAt = camera->mAt;
    sead::Vector3f cameraUp = camera->mUp;
    sead::Vector3f diff = cameraAt - cameraPos;

    ImGui::DragFloat3("Camera Pos", &cameraPos.x, 50.f, -0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Camera At", &cameraAt.x, 50.f, -0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    //ImGui::DragFloat3("Camera Up", &cameraUp.x, 50.f, -0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);

    float verticalCamAngle = DEG(atan2f(diff.y, sqrtf(diff.x * diff.x + diff.z * diff.z)));
    float horizontalCamAngle = DEG(atan2f(diff.z, diff.x));
    float camHAngle = atan2f(diff.z, diff.x);
    if (camHAngle < 0)
        camHAngle += M_PI * 2;

    ImGui::DragFloat("Vertical Cam Angle", &verticalCamAngle, 1.f, 0.f, 90.f, format, ImGuiSliderFlags_NoInput);
    ImGui::DragFloat("Horizontal Cam Angle", &horizontalCamAngle, 1.f, 0.f, 90.f, format, ImGuiSliderFlags_NoInput);

    float relAngleVel = hSpeedAngle - camHAngle - (M_PI / 2); // offset to move 0 to the right
    if (relAngleVel < 0)
        relAngleVel += M_PI * 2;
    relAngleVel = -relAngleVel + M_PI*2; // invert to conform normal anti-clockwise angle system

    float relVelAngleDeg = DEG(relAngleVel);
    ImGui::DragFloat("Rel Vel Angle", &relVelAngleDeg, 1.f, 0.f, 360.f, format, ImGuiSliderFlags_NoInput);

    //sead::Vector3f kidsPos = playerHak->mRecoverySafetyPoint->mSafetyPointPos;
    //ImGui::InputFloat3("Assist Pos", &kidsPos.x, "%.00f", ImGuiInputTextFlags_ReadOnly);
}


sead::Vector3f QuatToEuler(const sead::Quatf* quat) {
    // Check for null pointer
    if (!quat) {
        // Handle the error, e.g., return a default value or log an error
        return sead::Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Extract quaternion components
    f32 x = quat->z;
    f32 y = quat->y;
    f32 z = quat->x;
    f32 w = quat->w;

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

    return sead::Vector3f(yaw, pitch, roll);
}