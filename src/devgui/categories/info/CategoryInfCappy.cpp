#include "devgui/categories/info/CategoryInfCappy.h"

#include "al/Library/LiveActor/ActorMovementFunction.h"
#include "al/Library/LiveActor/ActorPoseKeeper.h"
#include "al/Library/Nerve/Nerve.h"
#include "al/Library/Nerve/NerveKeeper.h"
#include "al/Library/Nerve/NerveStateCtrl.h"
#include "al/Library/Nerve/NerveUtil.h"

#include "game/Player/PlayerActorBase.h"
#include "game/Player/PlayerActorHakoniwa.h"

#include "custom/game/Player/HackCap.h"

#include <cstdio>
#include <cxxabi.h>
#include <typeinfo>

#include "devgui/DevGuiManager.h"
#include "devgui/windows/StagePause/WindowStagePause.h"
#include "helpers/GetHelper.h"
#include "helpers/ImGuiHelper.h"
#include "helpers/MathHelper.h"
#include "imgui.h"
#include "math/seadVectorFwd.h"

#define DEG(X) X * 180 / M_PI

CategoryInfCappy::CategoryInfCappy(const char* catName, const char* catDesc, sead::Heap* heap) : CategoryBase(catName, catDesc, heap) {}

void CategoryInfCappy::updateCatDisplay() {
    PlayerActorBase* player = tryGetPlayerActor();

    if (!player) {
        ImGui::Text("Player does not exist!");
        return;
    }
    PlayerActorHakoniwa* playerHak = tryGetPlayerActorHakoniwa();
    HackCap* cappy = playerHak->mHackCap;

    /*
        Precision Slider
    */

    ImGui::SliderInt("Precision", &sliderValue, 0, 10);
    snprintf(format, sizeof(format), "%%.%df", static_cast<int>(sliderValue));

    ImGui::Checkbox("Use Degree", &mUseDeg);

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
        if (state) {
            const al::Nerve* stateNerve = state->state->getNerveKeeper()->getCurrentNerve();
            stateName = abi::__cxa_demangle(typeid(*state->state).name(), nullptr, nullptr, &status);
            stateNrvName = abi::__cxa_demangle(typeid(*stateNerve).name(), nullptr, nullptr, &status);
        }
    }

    if (cappy) {
        int status;
        al::NerveKeeper* nerveKeeper = cappy->getNerveKeeper();
        char* nerveName = nullptr;
        int prefixLen = 0;

        if (nerveKeeper) {
            const al::Nerve* currentNerve = nerveKeeper->getCurrentNerve();
            if (currentNerve) {
                nerveName = abi::__cxa_demangle(typeid(*currentNerve).name(), nullptr, nullptr, &status);
                prefixLen = nerveName[0] == '(' ? strlen("(anonymous namespace)::") : 0;
            }
        }

        ImGui::Text("CappyNrv: %s", nerveName + prefixLen);
        free(nerveName);
    }

    //------------------------------------------------------------------------------

    al::ActorPoseKeeperBase* pose = cappy->mPoseKeeper;
    StageScene* stageScene = tryGetStageScene();

    if (!pose)
        return;

    float hSpeed = al::calcSpeedH(cappy), vSpeed = al::calcSpeedV(cappy), speed = al::calcSpeed(cappy);
    float hSpeedAngle = atan2f(pose->getVelocityPtr()->z, pose->getVelocityPtr()->x);
    if (hSpeedAngle < 0)
        hSpeedAngle += M_PI * 2;

    float hSpeedAngleDeg = DEG(hSpeedAngle);

    static sead::Vector3f prevCappyVel = {0.0f, 0.0f, 0.0f};
    sead::Vector3f cappyVelDelta = pose->getVelocity() - prevCappyVel;

    ImGui::DragFloat3("Trans", &pose->mTrans.x, 50.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Velocity", &pose->getVelocityPtr()->x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);
    ImGui::DragFloat3("Vel Delta", &cappyVelDelta.x, 1.f, 0.f, 0.f, format, ImGuiSliderFlags_NoRoundToFormat);

    ImGui::DragFloat("Vel Angle", mUseDeg ? &hSpeedAngleDeg : &hSpeedAngle, 1.f, 0.f, 360.f, format, ImGuiSliderFlags_NoInput);
    WindowStagePause* win = DevGuiManager::instance()->getWindow<WindowStagePause>(windowNameStagePause);
    prevCappyVel = win->getStagePaused() ? prevCappyVel : pose->getVelocity();
    ImGuiHelper::Quat("Cappy Quaternion", pose->getQuatPtr());

    sead::Vector3f cappyEulerAngles = MathHelper::QuatToEuler(pose->getQuatPtr());
    sead::Vector3f cappyRotDeg = sead::Vector3f(DEG(cappyEulerAngles.x), DEG(cappyEulerAngles.y), DEG(cappyEulerAngles.z));
    ImGui::DragFloat3("Euler", mUseDeg ? &cappyRotDeg.x : &cappyEulerAngles.x, 1.f, -1.f, 1.f, format, ImGuiSliderFlags_NoRoundToFormat);

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
}
