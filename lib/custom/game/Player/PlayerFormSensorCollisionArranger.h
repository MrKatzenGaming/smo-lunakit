#pragma once

#include "sead/math/seadVector.h"

#include "Library/LiveActor/LiveActor.h"

#include "PlayerColliderHakoniwa.h"
#include "PlayerHackKeeper.h"

class IPlayerModelChanger;

class PlayerFormSensorCollisionArranger {
public:
    PlayerFormSensorCollisionArranger(al::LiveActor*, PlayerColliderHakoniwa*, const IPlayerModelChanger*, const PlayerHackKeeper*);
    void setFormModel3D(void);
    void setFormModel2D(void);
    void setFormActionSquat(void);
    void setFormActionStandup(void);
    void setFormActionWallGrab(const sead::Vector3f&);
    void setFormActionGrabCeil(const sead::Vector3f&);
    void setFormActionPoleClimb(const sead::Vector3f&);
    void setFormActionSwim(void);
    void setFormActionHack(void);
    void setFormActionBind(bool);
    void setFormActionRecovery(void);
    void setFormActionAbyss(const sead::Vector3f&);
    void setFormActionDead(void);
    void setFormAttackSensorNone(void);
    void setFormAttackSensorSpin(void);
    void setFormAttackSensorTornado(void);
    void setCollisionShapeOffsetGround(float);
    const char* getHeadSensorName(void) const;
    sead::Vector3f* getHeadPos(void) const;
    float getHeadRadius(void) const;
    sead::Vector3f* getBodyPos(void) const;
    bool isEnableSafetyPointForm(void) const;
    void update(void);
    void syncForm(void);
    void validateAttackSensor(void);
};
