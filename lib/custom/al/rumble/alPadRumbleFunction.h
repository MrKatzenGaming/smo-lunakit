#pragma once

#include "Library/Layout/LayoutActor.h"
#include "Library/LiveActor/LiveActor.h"

#include "PadRumbleParam.h"

namespace al {
class PadRumbleDirector;
}

namespace alPadRumbleFunction {
al::PadRumbleDirector* getPadRumbleDirector(const al::LiveActor*);
al::PadRumbleDirector* getPadRumbleDirector(const al::LayoutActor*);
void startPadRumble(al::PadRumbleDirector*, const sead::Vector3<float>&, const char*, float, float, int);
void startPadRumbleWithParam(al::PadRumbleDirector*, const sead::Vector3<float>&, const char*, const al::PadRumbleParam&, int);
void startPadRumble(const al::LiveActor*, const char*, float, float, int);
void startPadRumblePos(const al::LiveActor*, const sead::Vector3<float>&, const char*, float, float, int);
void startPadRumbleWithParam(const al::LiveActor*, const sead::Vector3<float>&, const char*, const al::PadRumbleParam&, int);
void startPadRumbleNo3D(al::PadRumbleDirector*, const char*, int);
void startPadRumbleNo3DWithParam(al::PadRumbleDirector*, const char*, const al::PadRumbleParam&, int);
void startPadRumbleNo3DWithParam(al::PadRumbleDirector*, const char*, float, float, float, float, int);
void startPadRumbleNo3D(const al::LiveActor*, const char*, int);
void startPadRumbleNo3DWithParam(const al::LiveActor*, const char*, const al::PadRumbleParam&, int);
void startPadRumbleNo3DWithParam(const al::LiveActor*, const char*, float, float, float, float, int);
void stopPadRumbleOneTime(al::PadRumbleDirector*, const char*, int);
void stopPadRumbleOneTime(const al::LiveActor*, const char*, int);
void startPadRumbleLoop(al::PadRumbleDirector*, const char*, const sead::Vector3<float>*, float, float, int);
void startPadRumbleLoopWithParam(al::PadRumbleDirector*, const char*, const sead::Vector3<float>*, const al::PadRumbleParam&, int);
void startPadRumbleLoop(const al::LiveActor*, const char*, const sead::Vector3<float>*, float, float, int);
void startPadRumbleLoopWithParam(const al::LiveActor*, const char*, const sead::Vector3<float>*, const al::PadRumbleParam&, int);
void startPadRumbleLoopNo3D(al::PadRumbleDirector*, const char*, const sead::Vector3<float>*, int);
void startPadRumbleLoopNo3DWithParam(al::PadRumbleDirector*, const char*, const sead::Vector3<float>*, const al::PadRumbleParam&, int);
void startPadRumbleLoopNo3D(const al::LiveActor*, const char*, const sead::Vector3<float>*, int);
void startPadRumbleLoopNo3DWithParam(const al::LiveActor*, const char*, const sead::Vector3<float>*, const al::PadRumbleParam&, int);
void stopPadRumbleLoop(al::PadRumbleDirector*, const char*, const sead::Vector3<float>*, int);
void stopPadRumbleLoop(const al::LiveActor*, const char*, const sead::Vector3<float>*, int);
void checkIsAlivePadRumbleLoop(al::PadRumbleDirector*, const char*, const sead::Vector3<float>*, int);
void checkIsAlivePadRumbleLoop(const al::LiveActor*, const char*, const sead::Vector3<float>*, int);
void startPadRumbleLoopControlable(const al::LiveActor*, const char*, const sead::Vector3<float>*, int);
void changePadRumbleLoopVolmue(const al::LiveActor*, const char*, const sead::Vector3<float>*, float, float, int);
void changePadRumbleLoopVolmueEaseInRange(const al::LiveActor*, const char*, const sead::Vector3<float>*, float, float, float, float, float, int);
void changePadRumbleLoopPitch(const al::LiveActor*, const char*, const sead::Vector3<float>*, float, float, int);
void startPadRumbleDirectValue(const al::LiveActor*, float, float, float, float, float, float, int);
void stopPadRumbleDirectValue(const al::LiveActor*, int);
void startPadRumbleWithVolume(const al::LiveActor*, const char*, float, float, int);
void startPadRumbleWithVolume(al::PadRumbleDirector*, const char*, float, float, int);
void makePadRumbleParamNearFarVolume(al::PadRumbleParam*, float, float, float);
void makePadRumbleParamNearFarVolumeLR(al::PadRumbleParam*, float, float, float, float);
void makePadRumbleParamNearFarVolumePitch(al::PadRumbleParam*, float, float, float, float);
void makePadRumbleParamNearFarVolumePitchLR(al::PadRumbleParam*, float, float, float, float, float, float);

};  // namespace alPadRumbleFunction
