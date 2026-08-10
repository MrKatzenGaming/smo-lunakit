#include "devgui/categories/info/CategoryInfPlayer.h"

#include "sead/gfx/seadCamera.h"
#include "sead/math/seadVectorFwd.h"

#include "al/Library/Camera/CameraUtil.h"
#include "al/Library/LiveActor/ActorMovementFunction.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateCtrl.h"

#include "game/Player/PlayerActorBase.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerAnimator.h"
#include "game/Player/PlayerFunction.h"
#include "game/Util/DemoUtil.h"
#include "game/Util/PlayerUtil.h"

#include "custom/game/Player/HackCap.h"

#include <cstdio>
#include <cxxabi.h>
#include <typeinfo>

#include "devgui/DevGuiManager.h"
#include "devgui/windows/WindowStagePause.h"
#include "helpers/GetHelper.h"
#include "helpers/ImGuiHelper.h"
#include "helpers/MathHelper.h"
#include "imgui.h"

#define DEG(X) X * 180 / M_PI

namespace {
sead::Vector3f calcMagnitudes(const sead::Vector3f& vec) {
    sead::Vector3f newVec;
    newVec.x = sqrtf(vec.z * vec.z + vec.x * vec.x);
    newVec.y = vec.y;
    newVec.z = sqrtf(newVec.x * newVec.x + newVec.y * newVec.y);
    return newVec;
}

sead::Vector3f calculateVelocity(const sead::Vector3f& oldTrans, const sead::Vector3f& newTrans) {
    sead::Vector3f velocity;
    velocity.x = newTrans.x - oldTrans.x;
    velocity.y = newTrans.y - oldTrans.y;
    velocity.z = newTrans.z - oldTrans.z;
    return velocity;
}
}  // namespace

CategoryInfPlayer::CategoryInfPlayer(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryInfPlayer::updateCatDisplay() {
    PlayerActorBase* player = tryGetPlayerActor();

    if (!player) {
        ImGui::Text("Player does not exist!");
        return;
    }

    /*
        Precision Slider
    */

    ImGui::SliderInt("Precision", &sliderValue, 0, 10);
    snprintf(format, sizeof(format), "%%.%df", static_cast<int>(sliderValue));

    ImGui::Checkbox("Use Degree", &mUseDeg);

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
        if (state) {
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

    if (stateName && stateNrvName) {
        ImGui::Text("State: %s", stateName);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Current type of action\nthe player is performing");

        ImGui::Text("State Nrv: %s", stateNrvName + 23 + strlen(stateName) + 3);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Basically the sub-action\nCurrent nerve of the state");

        free(stateName);
        free(stateNrvName);
    }

    ImGui::Text("NRV-Step: %d", player->getNerveKeeper()->getCurrentStep());

    /*
        // PLAYER ANIMATIONS (ONLY IF PlayerActorHakoniwa)
    */

    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    if (!playerHak) {
        ImGui::TextDisabled("Cannot display additional info, not Hakoniwa");
        return;
    }

    PlayerAnimator* anim = playerHak->mAnimator;
    ImGui::Text("Anim: %s (%.00f/%.00f)", anim->mCurAnim.cstr(), anim->getAnimFrame(), anim->getAnimFrameMax());
    ImGui::Text("Sub Anim: %s (%.00f/%.00f)", anim->mCurSubAnim.cstr(), anim->getSubAnimFrame(), anim->getSubAnimFrameMax());

    //-----------------------------------------------------------------------------------------
    if (!playerHak)
        return;

    al::ActorPoseKeeperBase* pose = playerHak->mPoseKeeper;
    StageScene* stageScene = tryGetStageScene();

    if (!pose)
        return;

    float hSpeed = al::calcSpeedH(playerHak), vSpeed = al::calcSpeedV(playerHak), speed = al::calcSpeed(playerHak);
    float hSpeedAngle = atan2f(pose->getVelocityPtr()->x, pose->getVelocityPtr()->z);
    if (hSpeedAngle < 0)
        hSpeedAngle += M_PI * 2;

    static sead::Vector3f prevPlayerVel = {0.0f, 0.0f, 0.0f};
    sead::Vector3f playerVelDelta = pose->getVelocity() - prevPlayerVel;

    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    if (win)
        prevPlayerVel = win->getStagePaused() ? prevPlayerVel : pose->getVelocity();

    sead::Vector3f playerRot = MathHelper::QuatToEuler(pose->getQuatPtr());

    const sead::LookAtCamera* camera = &al::getLookAtCamera(stageScene, 0);
    if (!camera) {
        ImGui::TextDisabled("No camera found");
        return;
    }
    sead::Vector3f cameraPos = camera->mPos;
    sead::Vector3f cameraAt = camera->mAt;
    sead::Vector3f cameraUp = camera->mUp;
    sead::Vector3f camDiff = cameraAt - cameraPos;

    float verticalCamAngle = DEG(atan2f(camDiff.y, sqrtf(camDiff.x * camDiff.x + camDiff.z * camDiff.z)));
    float camHAngle = atan2f(camDiff.x, camDiff.z);
    if (camHAngle < 0)
        camHAngle += M_PI * 2;

    float relAngleVel = hSpeedAngle - camHAngle + (M_PI / 2);  // offset to move 0 to the right
    if (relAngleVel < 0)
        relAngleVel += M_PI * 2;
    // relAngleVel = -relAngleVel + M_PI * 2;  // invert to conform normal anti-clockwise angle system

    float relRotAngle = playerRot.y - camHAngle + (M_PI / 2);  // offset to move 0 to the right
    if (relRotAngle < 0)
        relRotAngle += M_PI * 2;
    // relRotAngle = -relRotAngle + M_PI*2; // invert to conform normal anti-clockwise angle system

    if (ImGui::CollapsingHeader("Normal")) {
        ImGui::DragFloat3("Trans", &pose->mTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Velocity", &pose->getVelocityPtr()->x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Vel Delta", &playerVelDelta.x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);

        snprintf(textBuffer, sizeof(textBuffer), "Speed H: %s", format);
        ImGui::Text(textBuffer, hSpeed);
        ImGui::SameLine();
        snprintf(textBuffer, sizeof(textBuffer), "V: %s", format);
        ImGui::Text(textBuffer, vSpeed);
        ImGui::SameLine();
        snprintf(textBuffer, sizeof(textBuffer), "S: %s", format);
        ImGui::Text(textBuffer, speed);

        snprintf(textBuffer, sizeof(textBuffer), "H Speed Angle: %s", format);
        ImGui::Text(textBuffer, mUseDeg ? DEG(hSpeedAngle) : hSpeedAngle);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = Towards Z Axis");

        sead::Vector3f playerRotDeg = {0, 0, 0};
        playerRotDeg.x = DEG(playerRot.x);
        playerRotDeg.y = DEG(playerRot.y);
        playerRotDeg.z = DEG(playerRot.z);

        ImGuiHelper::Quat("Player Quaternion", pose->getQuatPtr());
        ImGui::DragFloat3("Euler", mUseDeg ? &playerRotDeg.x : &playerRot.x, 1.f, -1.f, 1.f, format, ImGuiSliderFlags_NoRoundToFormat);

        ImGui::DragFloat3("Camera Pos", &cameraPos.x, 50.f, -0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Camera At", &cameraAt.x, 50.f, -0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        // ImGui::DragFloat3("Camera Up", &cameraUp.x, 50.f, -0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        snprintf(textBuffer, sizeof(textBuffer), "Cam Angle V: %s", format);
        ImGui::Text(textBuffer, verticalCamAngle);
        ImGui::SameLine();
        snprintf(textBuffer, sizeof(textBuffer), "H: %s", format);
        ImGui::Text(textBuffer, mUseDeg ? DEG(camHAngle) : camHAngle);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = Facing Z Axis");

        snprintf(textBuffer, sizeof(textBuffer), "Rel. Vel. Angle: %s", format);
        ImGui::Text(textBuffer, mUseDeg ? DEG(relAngleVel) : relAngleVel);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = Right");
        snprintf(textBuffer, sizeof(textBuffer), "Rel. Rot. Angle: %s", format);
        ImGui::Text(textBuffer, mUseDeg ? DEG(relRotAngle) : relRotAngle);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = Right");
        // sead::Vector3f kidsPos = playerHak->mRecoverySafetyPoint->mSafetyPointPos;
        // ImGui::InputFloat3("Assist Pos", &kidsPos.x, "%.00f", ImGuiInputTextFlags_ReadOnly);
    }

    if (ImGui::CollapsingHeader("Observed")) {
        static sead::Vector3f prevPlayerTrans = {0.0f, 0.0f, 0.0f};
        sead::Vector3f observedVel = calculateVelocity(prevPlayerTrans, pose->getTrans());
        sead::Vector3f observedSpeed = calcMagnitudes(observedVel);

        float observedHSpeedAngle = atan2f(observedVel.x, observedVel.z);
        if (observedHSpeedAngle < 0)
            observedHSpeedAngle += M_PI * 2;

        prevPlayerTrans = win->getStagePaused() ? prevPlayerTrans : pose->getTrans();

        float observedRelAngleVel = observedHSpeedAngle - camHAngle + (M_PI / 2);
        if (observedRelAngleVel < 0)
            observedRelAngleVel += M_PI * 2;

        ImGui::PushID("Observed");
        ImGui::DragFloat3("Velocity", &observedVel.x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);

        snprintf(textBuffer, sizeof(textBuffer), "Speed H: %s", format);
        ImGui::Text(textBuffer, observedSpeed.x);
        ImGui::SameLine();
        snprintf(textBuffer, sizeof(textBuffer), "V: %s", format);
        ImGui::Text(textBuffer, observedSpeed.y);
        ImGui::SameLine();
        snprintf(textBuffer, sizeof(textBuffer), "S: %s", format);
        ImGui::Text(textBuffer, observedSpeed.z);

        snprintf(textBuffer, sizeof(textBuffer), "H Speed Angle: %s", format);
        ImGui::Text(textBuffer, mUseDeg ? DEG(observedHSpeedAngle) : observedHSpeedAngle);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = Towards Z Axis");

        snprintf(textBuffer, sizeof(textBuffer), "Rel. Vel. Angle: %s", format);
        ImGui::Text(textBuffer, mUseDeg ? DEG(observedRelAngleVel) : observedRelAngleVel);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("0 = Right");
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Relative Axes")) {
        if (!playerHak->mHackCap) {
            ImGui::Text("Cappy does not exist!");
            return;
        }

        sead::Vector3f playerTrans = pose->getTrans();
        sead::Vector3f playerVel = pose->getVelocity();
        sead::Vector3f cappyTrans = playerHak->mHackCap->getPoseKeeper()->getTrans();
        sead::Vector3f playerEulerAngles = MathHelper::QuatToEuler(pose->getQuatPtr());
        sead::Vector3f cappyEulerAngles = MathHelper::QuatToEuler(playerHak->mHackCap->getPoseKeeper()->getQuatPtr());

        sead::Vector3f playerABCTrans = transformToRelativeAxes(playerTrans, true);
        sead::Vector3f playerABCDistances = calcMagnitudes(playerABCTrans);
        sead::Vector3f playerABCVel = transformToRelativeAxes(playerVel, false);
        sead::Vector3f cappyABCTrans = transformToRelativeAxes(cappyTrans, true);
        sead::Vector3f cappyABCDistances = calcMagnitudes(cappyABCTrans);
        sead::Vector3f playerABCEulerAngles = transformEulerAnglesToRelativeAxes(playerEulerAngles);
        sead::Vector3f cappyABCEulerAngles = transformEulerAnglesToRelativeAxes(cappyEulerAngles);
        sead::Vector3f playerABCRot = sead::Vector3f(DEG(playerABCEulerAngles.x), DEG(playerABCEulerAngles.y), DEG(playerABCEulerAngles.z));
        sead::Vector3f cappyABCRot = sead::Vector3f(DEG(cappyABCEulerAngles.x), DEG(cappyABCEulerAngles.y), DEG(cappyABCEulerAngles.z));
        float relAngleDeg = DEG(mRelativeAngle);

        ImGui::PushID("RelAx");
        ImGui::DragFloat3("Origin", &mRelativeTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat("Rotation", mUseDeg ? &relAngleDeg : &mRelativeAngle, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::Checkbox("Fixed Origin/Rotation", &mFixedOrigin);

        if (mFixedOrigin) {
            ImGui::DragFloat3("Player Pos", &playerABCTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
            snprintf(textBuffer, sizeof(textBuffer), "Player Dist Origin H: %s", format);
            ImGui::Text(textBuffer, playerABCDistances.x);
            ImGui::SameLine();
            snprintf(textBuffer, sizeof(textBuffer), "S: %s", format);
            ImGui::Text(textBuffer, playerABCDistances.z);
        }

        ImGui::DragFloat3("Player Vel", &playerABCVel.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::DragFloat3("Player Rot", mUseDeg ? &playerABCRot.x : &playerABCEulerAngles.x, 50.f, 0.f, 0.f, format,
                          ImGuiSliderFlags_NoRoundToFormat);

        ImGui::DragFloat3("Cappy Pos", &cappyABCTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        snprintf(textBuffer, sizeof(textBuffer), "Cap Dist Origin H: %s", format);
        ImGui::Text(textBuffer, cappyABCDistances.x);
        ImGui::SameLine();
        snprintf(textBuffer, sizeof(textBuffer), "S: %s", format);
        ImGui::Text(textBuffer, cappyABCDistances.z);
        ImGui::DragFloat3("Cappy Rot", mUseDeg ? &cappyABCRot.x : &cappyABCEulerAngles.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
        ImGui::PopID();
    }
}

void CategoryInfPlayer::updateCat() {
    PlayerActorBase* player = tryGetPlayerActor();

    if (!player)
        return;

    sead::Vector3f euler = MathHelper::QuatToEuler(player->mPoseKeeper->getQuatPtr());
    if (!mFixedOrigin)
        setRelativeAxes(player->mPoseKeeper->getTrans(), euler.y);
}

sead::Vector3f CategoryInfPlayer::transformToRelativeAxes(sead::Vector3f vec, bool isTransVector) {
    if (isTransVector) {
        vec.x -= mRelativeTrans.x;
        vec.y -= mRelativeTrans.y;
        vec.z -= mRelativeTrans.z;
    }
    const double zxAngle = atan2f(vec.x, vec.z);
    const double caAngle = zxAngle - mRelativeAngle;
    const double magnitude = sqrtf(vec.z * vec.z + vec.x * vec.x);
    vec.z = magnitude * cosf(caAngle);
    vec.x = magnitude * sinf(caAngle);
    return vec;
}

sead::Vector3f CategoryInfPlayer::transformEulerAnglesToRelativeAxes(sead::Vector3f vec) {
    sead::Vector3f newVec;
    newVec.z = atanf(tanf(vec.z) * cosf(mRelativeAngle) + tanf(vec.x) * sinf(mRelativeAngle));
    newVec.x = atanf(tanf(vec.z) * cosf(mRelativeAngle + M_PI_2) + tanf(vec.x) * sinf(mRelativeAngle + M_PI_2));
    newVec.y = vec.y - mRelativeAngle;
    if (newVec.y < 0)
        newVec.y += M_PI * 2;
    return newVec;
}
