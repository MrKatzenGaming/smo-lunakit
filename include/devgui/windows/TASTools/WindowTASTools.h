#pragma once

#include "Library/LiveActor/ActorPoseKeeper.h"
#include "Library/Nerve/Nerve.h"
#include "Library/Nerve/NerveKeeper.h"

#include "custom/game/Player/HackCap.h"

#include "Player/PlayerActorHakoniwa.h"
#include "Player/PlayerAnimator.h"
#include "devgui/windows/WindowBase.h"
#include "math/seadQuat.h"
#include "math/seadVectorFwd.h"

__attribute__((used)) static const char* tasToolsWindowName = "TAS Tools";
void setupTasHooks();

class WindowTASTools : public WindowBase {
public:
    WindowTASTools(DevGuiManager* parent, const char* winName, bool isActiveByDefault);

    void updateWin() override;
    bool tryUpdateWinDisplay() override;

    void update();

    int mCurrPattern = Random;

    struct MofumofuPatternEntry {
        const char* typeStr;
        int target;
        bool reverse;
    };

    constexpr static const MofumofuPatternEntry mPatternEntries[22] = {
        {"Ghost", 0, false}, {"Nose", 0, true},     {"C", 1, false},       {"W", 1, true},       {"J", 2, false},        {"Medal", 2, true},
        {"Plane", 3, false}, {"5", 3, true},        {"Hangman", 4, false}, {"Spanish", 4, true}, {"Siblings", 5, false}, {"Snake", 5, true},
        {"8", 6, false},     {"Mushroom", 6, true}, {"Z", 7, false},       {"Tetris", 7, true},  {"Ear", 8, false},      {"Bomb", 8, true},
        {"Bird", 9, false},  {"L", 9, true},        {"O", 10, false},      {"Star", 10, true}};

private:
    enum MofumofuPattern {
        Random = 0,
        Ghost = 1,
        Nose = 2,
        C = 3,
        W = 4,
        J = 5,
        Medal = 6,
        Plane = 7,
        Five = 8,
        Hangman = 9,
        Spanish = 10,
        Siblings = 11,
        Snake = 12,
        Eight = 13,
        Mushroom = 14,
        Z = 15,
        Tetris = 16,
        Ear = 17,
        Bomb = 18,
        Bird = 19,
        L = 20,
        O = 21,
        Star = 22
    };

    static const int MAX_SAVED_STATES = 5;

    struct SaveState {
        const al::Nerve* PlayerNerve;
        const al::Nerve* CappyNerve;
        // al::NerveStateCtrl::State* CappyState;
        sead::Vector3f PlayerPosition;
        sead::Vector3f CappyPosition;
        sead::Vector3f PlayerVelocity;
        sead::Vector3f CappyVelocity;
        sead::Quatf PlayerRotation;
        sead::Quatf CappyRotation;
        int CappyNerveStep = 0;
        int PlayerNerveStep = 0;
        char mAnimName[0xff] = {0};
        char mSubAnimName[0xff] = {0};
        int mAnimFrame = 0;
        int mSubAnimFrame = 0;
        bool isCapVisible = false;
        bool mIsSaved = false;
    };

    const char* patterns[23] = {"Random", "Ghost", "Nose",     "C", "W",      "J",   "Medal", "Plane", "5", "Hangman", "Spanish", "Siblings",
                                "Snake",  "8",     "Mushroom", "Z", "Tetris", "Ear", "Bomb",  "Bird",  "L", "O",       "Star"};

    const char* mSelectedPattern = "Random";

    void drawDropdown(const char* header, const char* options[], const int totalOptions, const char** output);

    void updateCurrentPattern();

    void saveState(SaveState& state);
    void loadState(SaveState& state);
    SaveState states[MAX_SAVED_STATES];
    int mSelectedSaveSlot = 0;
    int timer = -1;

    al::NerveKeeper mNerveKeeper = al::NerveKeeper(nullptr, nullptr, 0);
    // PlayerAnimator mAnimator;
    al::ActorPoseKeeperTQGMSV mPoseKeeper;
    HackCap mHackCap =
        HackCap(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    al::NerveKeeper mNerveKeeperHackCap = al::NerveKeeper(nullptr, nullptr, 0);
    al::ActorPoseKeeperTQGMSV mPoseKeeperHackCap;
};
