#include "actors/Puppet/PuppetCapActor.h"

#include "al/Library/Effect/EffectSystemInfo.h"
#include "al/Library/LiveActor/ActorActionFunction.h"
#include "al/Library/LiveActor/ActorAnimFunction.h"
#include "al/Library/LiveActor/ActorClippingFunction.h"
#include "al/Library/LiveActor/ActorFlagFunction.h"
#include "al/Library/LiveActor/ActorInitFunction.h"
#include "al/Library/LiveActor/ActorInitUtil.h"
#include "al/Library/LiveActor/ActorModelFunction.h"
#include "al/Library/LiveActor/ActorPoseUtil.h"
#include "al/Library/LiveActor/ActorSensorUtil.h"
#include "al/Library/LiveActor/LiveActorFunction.h"

PuppetCapActor::PuppetCapActor(const char* name) : al::LiveActor(name) {}

void PuppetCapActor::init(const al::ActorInitInfo& initInfo) {
    sead::FixedSafeString<0x20> capModelName;

    PlayerFunction::createCapModelName(&capModelName, "Mario");

    PlayerFunction::initCapModelActorDemo(this, initInfo, capModelName.cstr());

    al::hideSilhouetteModelIfShow(this);

    al::initExecutorModelUpdate(this, initInfo);

    mJointKeeper = new HackCapJointControlKeeper();

    mJointKeeper->initCapJointControl(this);

    mModelCap = new al::LiveActor("装着表示用帽子");
    al::initActorWithArchiveName(mModelCap, initInfo, "MarioCap", "Display");
    if (mModelCap) {
        al::setEffectFollowPosPtr(mModelCap, "InvincibleStart", al::getTransPtr(mModelCap));
        al::setEffectFollowPosPtr(mModelCap, "Invincible", al::getTransPtr(mModelCap));
    }
    mJointKeeper->initDisplayCapJointControl(mModelCap);
    al::invalidateOcclusionQuery(mModelCap);
    mModelCap->makeActorDead();
    al::hideModelIfShow(mModelCap);
    al::hideSilhouetteModelIfShow(mModelCap);
    al::LiveActor* subActor1 = al::getSubActor(mModelCap, "デプスシャドウ");
    al::invalidateOcclusionQuery(subActor1);
    al::LiveActor* subActor2 = al::getSubActor(mModelCap, "ロックオン用キャップの目");
    al::invalidateOcclusionQuery(subActor2);

    al::invalidateHitSensors(this);
    makeActorDead();
}

void PuppetCapActor::initAfterPlacement() {
    al::LiveActor::initAfterPlacement();
}

void PuppetCapActor::movement() {
    al::LiveActor::movement();
}

void PuppetCapActor::control() {
    al::setScale(mModelCap, al::getScale(this));
    al::copyPose(this, mModelCap);
}

void PuppetCapActor::update() {
    al::LiveActor::calcAnim();
    al::LiveActor::movement();
}

void PuppetCapActor::startAction(const char* actName) {
    if (al::tryStartActionIfNotPlaying(this, actName)) {
        const char* curActName = al::getActionName(this);
        if (curActName) {
            if (al::isSklAnimExist(this, curActName))
                al::clearSklAnimInterpole(this);
        }
    }
}

void PuppetCapActor::appear() {
    mModelCap->makeActorAlive();
    al::LiveActor::appear();
}

void PuppetCapActor::kill() {
    mModelCap->makeActorDead();
    al::LiveActor::kill();
}
