#pragma once

#include <sead/math/seadQuat.h>
#include <sead/math/seadVector.h>
#include <sead/prim/seadSafeString.h>

#include "al/area/AreaObj.h"
#include "al/util/InputUtil.h"

namespace sead {
class Heap;
}

namespace nn::ui2d {
class TextureInfo;
}

class PlayerActorHakoniwa;

namespace al {

class LiveActor;

class PlayerHolder;

class IUseCamera;

class Projection;

class IUseLayout;

class ActorInitInfo;

class Scene;

class IUseAudioKeeper;

class SensorMsg;

class IUseSceneObjHolder;

class HitSensor;

class PlacementInfo;

// from Starlight's header files. TODO clean this up, and include them in the proper places

PlayerActorBase* getPlayerActor(const al::LiveActor*, int);

PlayerActorBase* getPlayerActor(const al::PlayerHolder*, int);

sead::Heap* getCurrentHeap();

int getSubActorNum(const al::LiveActor*);

al::LiveActor* getSubActor(const al::LiveActor*, int);

int getPlayerControllerPort(int);

const char* getActionName(const al::LiveActor*);

sead::Vector3f* getCameraPos(const al::IUseCamera*, int);

sead::Vector3f* getSensorPos(const al::LiveActor*, const char*);

float calcSpeed(const al::LiveActor*);

float calcSpeedH(const al::LiveActor*);

float calcSpeedV(const al::LiveActor*);

float getSensorRadius(const al::LiveActor*, const char*);

// setters

void setTransY(al::LiveActor*, float);

void setTrans(al::LiveActor*, const sead::Vector3f&);

void setScaleAll(al::LiveActor*, float);

void setGravity(const al::LiveActor*, const sead::Vector3f&);

void setFront(al::LiveActor*, const sead::Vector3f&);

void setQuat(al::LiveActor*, const sead::Quatf&);

void setPaneTexture(al::IUseLayout*, const char*, const nn::ui2d::TextureInfo*);

void setSensorFollowPosOffset(al::LiveActor*, const sead::Vector3f&);

// void setPaneString(al::IUseLayout *layout, char const *paneName, char16_t const *, ushort);

void setPaneStringFormat(al::IUseLayout* layout, const char* paneName, const char* format, ...);

void setVelocityZero(al::LiveActor*);

// calc functions

f32 calcDistance(const al::LiveActor*,
                 const al::LiveActor*);  // calculates distance between two actors

f32 calcDistance(const al::LiveActor*,
                 const sead::Vector3f&);  // calculates distance between an actor and a position in the world

// bools

bool isInAreaObj(const al::LiveActor*, const char*);

bool isInDeathArea(const al::LiveActor*);

bool getArg(int*, const al::ActorInitInfo&,
            const char*);  // gets an int argument from the actorinitinfo by a char* key

bool isActiveDemo(const al::Scene*);

bool isAreaTarget(const al::LiveActor*);

bool isSensorName(const al::HitSensor*, const char*);

// math

float powerIn(float base, float exponent);

float powerOut(float base, float exponent);

float squareIn(float value);

// misc

al::AreaObj* tryFindAreaObj(const al::LiveActor*, const char*);

bool tryGetAreaObjArg(int*, const al::AreaObj*, const char*);

bool tryGetAreaObjArg(float*, const al::AreaObj*, const char*);

bool tryGetAreaObjArg(bool*, const al::AreaObj*, const char*);

bool tryGetArg(int*, const al::ActorInitInfo&, const char*);

bool tryGetArg(float*, const al::ActorInitInfo&, const char*);

bool tryGetArg(bool*, const al::ActorInitInfo&, const char*);

bool tryGetStringArg(const char**, const al::ActorInitInfo*, const char*);

bool isEqualString(const char* stringA, const char* stringB);

void offCollide(al::LiveActor*);

void onCollide(al::LiveActor*);

void startAction(al::LiveActor*, const char*);

bool tryStartSe(const al::IUseAudioKeeper*, const sead::SafeStringBase<char>&);

void startSe(const al::IUseAudioKeeper*, const sead::SafeStringBase<char>&);

void startHitReaction(const al::LiveActor*, const char*);

void calcCameraUpDir(sead::Vector3f*, const al::IUseCamera*, int);

const unsigned char* tryGetBymlFromArcName(const sead::SafeStringBase<char>&, const sead::SafeStringBase<char>&);

void initActor(al::LiveActor*, const al::ActorInitInfo&);

bool isObjectName(const al::ActorInitInfo&, const char*);

bool isObjectName(const al::PlacementInfo&, const char*);

void invalidateHitSensors(al::LiveActor*);

void hideModelIfShow(al::LiveActor*);

void showModelIfHide(al::LiveActor*);

void hideModel(al::LiveActor*);

void showModel(al::LiveActor*);

}  // namespace al

namespace rs {
uint32_t getStageShineAnimFrame(const al::LiveActor*, const char*);

PlayerActorBase* getPlayerActor(const al::Scene*);

bool isInChangeStageArea(const al::LiveActor*, const sead::Vector3f*);

bool isInvalidChangeStage(const al::LiveActor*);

bool isMsgCapTouchWall(const al::SensorMsg*);

void buyCap(const al::IUseSceneObjHolder*, const char*);

int getActiveQuestNum(const al::IUseSceneObjHolder*);

int getActiveQuestNo(const al::IUseSceneObjHolder*);

const char* getActiveQuestLabel(const al::IUseSceneObjHolder*);

void requestShowHtmlViewer(const al::IUseSceneObjHolder*);
}  // namespace rs
