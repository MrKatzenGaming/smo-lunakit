#pragma once

#include "game/Player/PlayerAnimator.h"
#include "game/Player/PlayerConst.h"
#include "game/Player/PlayerInput.h"

#include "custom/game/Player/HackCap.h"

class PlayerTrigger;
class PlayerJudgePreInputCapThrow;
class PlayerCounterAfterCapCatch;

class PlayerSpinCapAttack {
public:
    PlayerSpinCapAttack(HackCap*, const PlayerConst*, const PlayerTrigger*, const PlayerInput*, const PlayerCounterAfterCapCatch*,
                        const PlayerJudgePreInputCapThrow*);
    void clearAttackInfo(void);
    void setupAttackInfo(void);
    void startCapSpinAttack(PlayerAnimator*, const PlayerInput*);
    void startCapSpinAttackAir(PlayerAnimator*, const PlayerInput*);
    void startCapSpinAttackSwim(PlayerAnimator*, const PlayerInput*);
    void startSpinSeparate(PlayerAnimator*, const PlayerInput*);
    void startSpinSeparateSwim(PlayerAnimator*, const PlayerInput*);
    void startSpinSeparateSwimSurface(PlayerAnimator*, const PlayerInput*);
    void startCapThrow(const sead::Vector3f& front, const sead::Vector3f& up, float speed, bool, const sead::Vector3f& unused);
    void attackSpinMsg(al::HitSensor*, al::HitSensor*);
    bool tryCancelCapState(PlayerAnimator*);
    bool tryStartCapSpinGroundMiss(PlayerAnimator*);
    bool tryStartCapSpinAirMiss(PlayerAnimator*);
    bool isCapSpinAttack(void) const;
    bool isValidAttackSensor(const PlayerAnimator*) const;
    bool isEnablePlaySpinCapMiss(const PlayerAnimator*) const;
    bool isSeparateSingleSpin(void) const;
    bool isThrowSwingRightDir(void) const;
    int getThrowFrameGround(void) const;
    int getThrowFrameAir(void) const;
    int getThrowFrameSwim(void) const;

    HackCap* mHackCap;                                  // 0x0
    PlayerConst* mPlayerConst;                          // 0x8
    PlayerTrigger* mPlayerTrigger;                      // 0x10
    PlayerInput* mPlayerInput;                          // 0x18
    PlayerCounterAfterCapCatch* mCounterAfterCapCatch;  // 0x20
    PlayerJudgePreInputCapThrow* jPreInputCapThrow;     // 0x28
};
