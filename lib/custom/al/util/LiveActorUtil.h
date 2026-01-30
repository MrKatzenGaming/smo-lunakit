#pragma once

#include <sead/math/seadMatrix.h>
#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>
#include <sead/prim/seadSafeString.h>

#include "Library/Collision/Collider.h"
#include "Library/Layout/LayoutActor.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Resource/ActorResource.h"
#include "Library/Resource/Resource.h"
#include "Library/Thread/FunctorV0M.h"

#include "game/Player/PlayerActorHakoniwa.h"

#include "layout/LayoutInitInfo.h"
#include "math/seadBoundBox.h"

namespace al {

void tryInitFixedModelGpuBuffer(const LiveActor*);
void offUpdateMovementEffectAudioCollisionSensor(const LiveActor*);
void hideModel(LiveActor*);
void hideModelIfShow(const LiveActor*);
void showModelIfHide(const LiveActor*);
void showModel(LiveActor*);
void setModelAlphaMask(const LiveActor*, float);
void resetPosition(const LiveActor*);
void onSyncClippingSubActor(LiveActor*, const LiveActor*);
void onSyncHideSubActor(LiveActor*, const LiveActor*);
void onSyncAlphaMaskSubActor(LiveActor*, const LiveActor*);
void setMaterialProgrammable(LiveActor*);
void startAction(LiveActor*, const char*);
void startAction(IUseLayoutAction*, const char*, const char*);
void startFreezeActionEnd(IUseLayoutAction*, const char*, const char*);
void startHitReaction(LiveActor*, const char*);
void invalidateClipping(LiveActor*);
void validateClipping(LiveActor*);
void setNerveAtActionEnd(LiveActor*, const al::Nerve*);
void updateMaterialCodeWater(LiveActor*);
void updateMaterialCodeWater(LiveActor*, bool);
void appearItem(const LiveActor*);
void turnToTarget(LiveActor*, const sead::Vector3f&, float);
void turnToTarget(LiveActor*, const al::LiveActor*, float);

void expandClippingRadiusByShadowLength(LiveActor*, sead::Vector3f*, float);
void setEffectFollowPosPtr(al::IUseEffectKeeper*, const char*, const sead::Vector3<float>*);
void invalidateOcclusionQuery(al::LiveActor*);

void initJointLocalXRotator(const LiveActor*, const float*, const char*);
void initJointLocalYRotator(const LiveActor*, const float*, const char*);
void initJointLocalZRotator(const LiveActor*, const float*, const char*);

void expandClippingRadiusByShadowLength(LiveActor*, sead::Vector3f*, float);

void initActorSceneInfo(al::LiveActor*, const al::ActorInitInfo&);
void initActorSRT(al::LiveActor*, const al::ActorInitInfo&);
void initActorModelKeeper(al::LiveActor*, const al::ActorInitInfo&, const char*, int, const char*);
void initPartialSklAnim(al::LiveActor*, int, int, int);
void addPartialSklAnimPartsListRecursive(al::LiveActor*, const char*, int);
bool tryGetActorInitFileIter(ByamlIter*, const al::Resource*, const char*, const char*);
void initActorEffectKeeper(al::LiveActor*, const al::ActorInitInfo&, const char*);
void initActorActionKeeper(al::LiveActor*, const al::ActorResource*, const char*, const char*);
void initActorClipping(al::LiveActor*, const al::ActorInitInfo&);
void onSyncAppearSubActor(al::LiveActor*, const al::LiveActor*);
al::Resource* getModelResource(const al::LiveActor*);

void initActorPoseTRSV(al::LiveActor*);
void initActorPoseTRMSV(al::LiveActor*);
void initActorPoseTRGMSV(al::LiveActor*);
void initActorPoseTFSV(al::LiveActor*);
void initActorPoseTFUSV(al::LiveActor*);
void initActorPoseTFGSV(al::LiveActor*);
void initActorPoseTQSV(al::LiveActor*);
void initActorPoseTQGSV(al::LiveActor*);
void initActorPoseTQGMSV(al::LiveActor*);
void initActorPoseT(al::LiveActor*, const sead::Vector3<float>&);
void initActorPoseTR(al::LiveActor*, const sead::Vector3<float>&, const sead::Vector3<float>&);

void initCreateActorWithPlacementInfo(LiveActor*, const al::ActorInitInfo&);
void initLayoutPartsActor(LayoutActor*, LayoutActor*, const LayoutInitInfo&, const char*, const char*);
void initMapPartsActor(LiveActor*, const al::ActorInitInfo&, const char*);
void initActorWithArchiveName(LiveActor*, const al::ActorInitInfo&, const sead::SafeString&, const char*);
void initJointControllerKeeper(const LiveActor*, int);
void initJointGlobalQuatController(const LiveActor*, const sead::Quatf*, const char*);

void appearBreakModelRandomRotateY(LiveActor*);

bool isNear(const LiveActor*, const LiveActor*, float);
bool isClipped(const LiveActor*);
bool isDead(const LiveActor*);
bool isAlive(const LiveActor*);
bool isHideModel(const LiveActor*);
bool isEffectEmitting(const IUseEffectKeeper*, const char*);
bool isActionEnd(const LiveActor*);
bool isActionPlaying(const LiveActor*, const char*);
bool isInvalidClipping(const LiveActor*);
bool isInWater(const LiveActor*);
bool isInWaterArea(const LiveActor*);
bool isOnGround(const LiveActor*, unsigned int);
bool isOnStageSwitch(const IUseStageSwitch*, const char*);
bool isValidStageSwitch(const IUseStageSwitch*, const char*);
bool isFallNextMove(const LiveActor*, const sead::Vector3f&, float, float);
bool isInDeathArea(const LiveActor*);
bool isCollidedFloorCode(const LiveActor*, const char*);
bool isNoCollide(const LiveActor*);
bool isNearPlayer(const LiveActor*, float);
bool isFallOrDamageCodeNextMove(const LiveActor*, const sead::Vector3f&, float, float);
bool isFaceToTargetDegreeH(const LiveActor*, const sead::Vector3f&, const sead::Vector3f&, float);

bool tryOnSwitchDeadOn(IUseStageSwitch*);
bool trySyncStageSwitchAppear(LiveActor*);
PlayerActorHakoniwa* tryFindNearestPlayerActor(const LiveActor*);
bool tryFindNearestPlayerPos(sead::Vector3f*, const LiveActor*);
bool tryAddRippleMiddle(const LiveActor*);
bool tryStartMclAnimIfNotPlaying(LiveActor*, const char*);
bool tryEmitEffect(IUseEffectKeeper* effectKeeper, const char* effectName, const sead::Vector3f* effectPosition);
bool tryStartActionIfNotPlaying(LiveActor*, const char*);

float getClippingRadius(const al::LiveActor*);
sead::Vector3f* getClippingObb(al::LiveActor*);
sead::Vector3f* getClippingCenterPos(const al::LiveActor*);

sead::Vector3f& getVelocity(const LiveActor*);
sead::Vector3f* getVelocityPtr(LiveActor*);
Collider* getActorCollider(const LiveActor*);

sead::Matrix34f* getJointMtxPtr(const LiveActor*, const char*);  // return type might be const

sead::Vector3f* getOnGroundNormal(const LiveActor*, u32);

void scaleVelocity(LiveActor*, float);
void scaleVelocityDirection(LiveActor*, const sead::Vector3f&, float);
void scaleVelocityExceptDirection(LiveActor*, const sead::Vector3f&, float);

void setClippingObb(LiveActor*, const sead::BoundBox3f&);
void setClippingInfo(LiveActor*, float, const sead::Vector3f*);
void setClippingNearDistance(LiveActor*, float);

void setTrans(LiveActor*, const sead::Vector3f&);
void setVelocity(LiveActor*, const sead::Vector3f&);
void setVelocity(LiveActor*, float, float, float);
void setVelocityX(LiveActor*, float);
void setVelocityY(LiveActor*, float);
void setVelocityZ(LiveActor*, float);
void setVelocityZero(LiveActor*);
void setVelocityBlowAttackAndTurnToTarget(LiveActor*, const sead::Vector3f&, float, float);
void setActionFrameRate(LiveActor*, float);
void setEffectAllScale(IUseEffectKeeper*, const char*, const sead::Vector3f&);

void addVelocityToGravityFittedGround(LiveActor*, float, unsigned int);
void addVelocityToGravity(LiveActor*, float);
void addVelocityToDirection(LiveActor*, const sead::Vector3f&, float);
void addVelocity(LiveActor*, const sead::Vector3f&);
void addVelocityX(LiveActor*, float);
void addVelocityY(LiveActor*, float);
void addVelocityZ(LiveActor*, float);

void calcFrontDir(sead::Vector3f*, const LiveActor*);
void calcQuat(sead::Quatf*, const LiveActor*);
void calcJointFrontDir(sead::Vector3f*, const LiveActor*, const char*);
void calcJointPos(sead::Vector3f*, const LiveActor*, const char*);

void makeQuatUpFront(sead::Quatf*, const sead::Vector3f&, const sead::Vector3f&);

void rotateQuatYDirDegree(LiveActor*, float);

f32* findActorParamF32(const LiveActor*, const char*);

s32* findActorParamS32(const LiveActor*, const char*);

bool listenStageSwitchOnAppear(IUseStageSwitch*, const al::FunctorBase& functor);
}  // namespace al

class SklAnimRetargettingInfo;

namespace rs {

sead::Vector3f* getPlayerPos(const al::LiveActor*);
SklAnimRetargettingInfo* createPlayerSklRetargettingInfo(al::LiveActor*, const sead::Vector3f&);

}  // namespace rs
