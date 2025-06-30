#include "program/devgui/windows/TASTools/WindowTASTools.h"
#include "al/util.hpp"
#include "game/Player/Attacks/PlayerSpinCapAttack.h"
#include "game/Player/HackCap.h"
#include "game/Player/PlayerModelHolder.h"
#include "game/System/GameDataFunction.h"
#include "Library/LiveActor/ActorPoseKeeper.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/LiveActor/LiveActorFlag.h"
#include "Library/LiveActor/SubActorKeeper.h"
#include "Player/PlayerActorHakoniwa.h"
#include "Player/PlayerAnimator.h"
#include "Player/PlayerModelHolder.h"
#include "devgui/DevGuiManager.h"

#include "al/util/LiveActorUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util/RandomUtil.h"

#include "helpers/GetHelper.h"
#include "imgui.h"
#include "logger/Logger.hpp"
#include "math/seadVectorFwd.h"
#include "util/LiveActorUtil.h"

WindowTASTools::WindowTASTools(DevGuiManager* parent, const char* winName, bool isActiveByDefault) : WindowBase(parent, winName, isActiveByDefault) {
    setupTasToolsHooks();
}

void WindowTASTools::updateWin() {}

void WindowTASTools::update() {
    if (timer != -1) {
        timer++;
    }
    if (timer == 2) {
        PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();
        if (!player) return;
        al::LiveActor* cappy = player->mHackCap;
        if (!cappy) return;

        player->getNerveKeeper()->mStep = states[mSelectedSaveSlot].PlayerNerveStep;
        cappy->getNerveKeeper()->mStep = states[mSelectedSaveSlot].CappyNerveStep;
        timer = -1;
    }
}

bool WindowTASTools::tryUpdateWinDisplay() {
    if (!WindowBase::tryUpdateWinDisplay()) return false;

    drawDropdown("Wiggler Pattern", patterns, 23, &mSelectedPattern);
    ImGui::Text("Current Pattern: %s", mSelectedPattern);
    ImGui::Text("Current Pattern ID: %d", mCurrPattern);
    if (ImGui::Button("Save State")) {
        saveState(states[mSelectedSaveSlot]);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load State")) {
        loadState(states[mSelectedSaveSlot]);
    }

    ImGui::SliderInt("Save Slot", &mSelectedSaveSlot, 0, MAX_SAVED_STATES - 1);
    SaveState currentState = states[mSelectedSaveSlot];
    if (currentState.mIsSaved) {
        ImGui::Text("State %d: %s", mSelectedSaveSlot, currentState.mIsSaved ? "Saved" : "Not Saved");
        ImGui::Text("Cappy Pos: %.3f, %.3f, %.3f", currentState.CappyPosition.x, currentState.CappyPosition.y, currentState.CappyPosition.z);
        ImGui::Text("Cappy Vel: %.3f, %.3f, %.3f", currentState.CappyVelocity.x, currentState.CappyVelocity.y, currentState.CappyVelocity.z);
        ImGui::Text(
            "Cappy Rot: %.3f, %.3f, %.3f, %.3f", currentState.CappyRotation.x, currentState.CappyRotation.y, currentState.CappyRotation.z,
            currentState.CappyRotation.w
        );
        ImGui::Text("Cappy Nerve Step: %d", currentState.CappyNerveStep);
        ImGui::Separator();
        ImGui::Text("Player Pos: %.3f, %.3f, %.3f", currentState.PlayerPosition.x, currentState.PlayerPosition.y, currentState.PlayerPosition.z);
        ImGui::Text("Player Vel: %.3f, %.3f, %.3f", currentState.PlayerVelocity.x, currentState.PlayerVelocity.y, currentState.PlayerVelocity.z);
        ImGui::Text(
            "Player Rot: %.3f, %.3f, %.3f, %.3f", currentState.PlayerRotation.x, currentState.PlayerRotation.y, currentState.PlayerRotation.z,
            currentState.PlayerRotation.w
        );
        ImGui::Text("Player Nerve Step: %d", currentState.PlayerNerveStep);
    }

    return true;
}

void WindowTASTools::drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output) {
    if (ImGui::BeginCombo(header, *output, 0)) {
        for (int n = 0; n < totalOptions; n++) {
            bool is_selected = (*output == options[n]);
            if (ImGui::Selectable(options[n], is_selected)) {
                *output = options[n];
                updateCurrentPattern();
            }
            if (is_selected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
}

bool isPatternReverse() {
    WindowTASTools* tools = (WindowTASTools*)DevGuiManager::instance()->getWindow("TAS Tools");
    bool b = al::isHalfProbability();
    if (tools->mCurrPattern != 0) b = tools->mPatternEntries[tools->mCurrPattern - 1].reverse == 1;
    return b;
}

int getMofumofuTarget(int a) {
    WindowTASTools* tools = (WindowTASTools*)DevGuiManager::instance()->getWindow("TAS Tools");
    int r = al::getRandom(a);
    if (tools->mCurrPattern != 0) r = tools->mPatternEntries[tools->mCurrPattern - 1].target;
    return r;
}

void WindowTASTools::updateCurrentPattern() {
    for (int i = 0; i < 23; ++i) {
        if (strcmp(mSelectedPattern, patterns[i]) == 0) {
            mCurrPattern = i;
            return;
        }
    }
    // If not found, default to Random
    mCurrPattern = Random;
}

void WindowTASTools::setupTasToolsHooks() {
    Logger::log("Setting up TAS Tools hooks\n");
    exl::patch::CodePatcher(0x0b07a8).BranchLinkInst((void*)isPatternReverse);
    exl::patch::CodePatcher(0x0b07f8).BranchLinkInst((void*)getMofumofuTarget);
}

void WindowTASTools::saveState(SaveState& state) {
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();
    if (!player) return;
    al::LiveActor* cappy = player->mHackCap;
    if (!cappy) return;

    Logger::log("Saving state\n");
    state.CappyNerve = al::getCurrentNerve(cappy);
    state.CappyPosition = al::getTrans(cappy);
    state.CappyVelocity = al::getVelocity(cappy);
    state.CappyRotation = al::getQuat(cappy);
    state.CappyNerveStep = cappy->getNerveKeeper()->getCurrentStep();
    state.isCapVisible = !player->mHackCap->mFlags->isModelVisible;

    state.PlayerNerve = al::getCurrentNerve(player);
    state.PlayerPosition = al::getTrans(player);
    state.PlayerVelocity = al::getVelocity(player);
    state.PlayerRotation = al::getQuat(player);
    state.PlayerNerveStep = player->getNerveKeeper()->getCurrentStep();
    strncpy(state.mAnimName, player->mAnimator->mCurAnim.cstr(), sizeof(state.mAnimName) - 1);
    strncpy(state.mSubAnimName, player->mAnimator->mCurSubAnim.cstr(), sizeof(state.mSubAnimName) - 1);
    state.mAnimFrame = player->mAnimator->getAnimFrame();
    state.mSubAnimFrame = player->mAnimator->getSubAnimFrame();

    Logger::log("State saved successfully\n");
    state.mIsSaved = true;

    memcpy(&mAnimator, player->mAnimator, sizeof(PlayerAnimator));
    memcpy(&mNerveKeeper, player->getNerveKeeper(), sizeof(al::NerveKeeper));
    memcpy(&mPoseKeeper, player->getPoseKeeper(), sizeof(al::ActorPoseKeeperTQGMSV));
    memcpy(&mHackCap, player->mHackCap, sizeof(HackCap));
    memcpy(&mNerveKeeperHackCap, player->mHackCap->getNerveKeeper(), sizeof(al::NerveKeeper));
    memcpy(&mPoseKeeperHackCap, player->mHackCap->getPoseKeeper(), sizeof(al::ActorPoseKeeperTQGMSV));

    return;
}

void WindowTASTools::loadState(SaveState& state) {
    PlayerActorHakoniwa* player = tryGetPlayerActorHakoniwa();
    if (!player) return;
    al::LiveActor* cappy = player->mHackCap;
    HackCap* hackCap = (HackCap*)cappy;
    if (!cappy) return;

    Logger::log("Loading state\n");
    if (!state.mIsSaved) {
        Logger::log("State not saved\n");
        return;
    }

    player->mSpinCapAttack->startCapThrow(sead::Vector3f(), sead::Vector3f(), 0.0f, false, sead::Vector3f());

    // hackCap->startPuppet();
    // // al::hideModel(al::getSubActor(player, "頭"));
    // al::setNerve(cappy, state.CappyNerve);
    // cappy->getNerveKeeper()->mStep = state.CappyNerveStep;

    // al::setTrans(cappy, states[mSelectedSaveSlot].CappyPosition);
    // al::setVelocity(cappy, states[mSelectedSaveSlot].CappyVelocity);
    // al::updatePoseQuat(cappy, states[mSelectedSaveSlot].CappyRotation);
    // hackCap->endPuppet();

    memcpy(player->mHackCap, &mHackCap, sizeof(HackCap));
    memcpy(player->mHackCap->getNerveKeeper(), &mNerveKeeperHackCap, sizeof(al::NerveKeeper));
    memcpy(player->mHackCap->getPoseKeeper(), &mPoseKeeperHackCap, sizeof(al::ActorPoseKeeperTQGMSV));
    // player->endDemoPuppetable();
    hackCap->mFlags->isModelVisible = true;

    player->startPlayerPuppet();
    al::setNerve(player, state.PlayerNerve);
    player->getNerveKeeper()->mStep = state.PlayerNerveStep;

    // player->mAnimator->startSubAnim(state.mSubAnimName);
    player->mAnimator->endSubAnim();
    player->mAnimator->startAnim(state.mAnimName);
    player->mAnimator->setAnimFrame(state.mAnimFrame);
    // player->mAnimator->setSubAnimFrame(state.mSubAnimFrame);

    al::setTrans(player, states[mSelectedSaveSlot].PlayerPosition);
    al::setVelocity(player, states[mSelectedSaveSlot].PlayerVelocity);
    al::updatePoseQuat(player, states[mSelectedSaveSlot].PlayerRotation);

    player->endPlayerPuppet();
    hackCap->hide(true);
    hackCap->hidePuppetCap();

    timer = 0;

    // memcpy(player->mAnimator, &mAnimator, sizeof(PlayerAnimator));
    // memcpy(player->getNerveKeeper(), &mNerveKeeper, sizeof(al::NerveKeeper));
    // memcpy(player->getPoseKeeper(), &mPoseKeeper, sizeof(al::ActorPoseKeeperTQGMSV));

    return;
}
