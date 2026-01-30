#pragma once

#include "sead/math/seadVector.h"

#include "al/camera/CameraDirector.h"

#include "types.h"

namespace al {

class SceneCameraInfo;

uint calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, float);
uint calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* output, const al::SceneCameraInfo*, const sead::Vector2f&, float, int);
uint calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, const sead::Vector3f&);
uint calcCameraPosToWorldPosDirFromScreenPos(sead::Vector3f* output, const al::SceneCameraInfo*, const sead::Vector2f&, const sead::Vector3f&, int);
void calcCameraPosToWorldPosDirFromScreenPosSub(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, float);
void calcCameraPosToWorldPosDirFromScreenPosSub(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, const sead::Vector3f&);

void calcLayoutPosFromScreenPos(sead::Vector2f* output, const sead::Vector2f&);
void calcLayoutPosFromWorldPosWithClampByScreen(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector3f&);

void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* output, sead::Vector3f*, const al::IUseCamera*, const sead::Vector2f&, float, float);
void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* output, sead::Vector3f*, const al::IUseCamera*, const sead::Vector2f&);
void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* output, sead::Vector3f*, const al::SceneCameraInfo*, const sead::Vector2f&, float, float,
                                           int);
void calcLineCameraToWorldPosFromScreenPos(sead::Vector3f* output, sead::Vector3f*, const al::SceneCameraInfo*, const sead::Vector2f&, int);
void calcLineCameraToWorldPosFromScreenPosSub(sead::Vector3f* output, sead::Vector3f*, const al::IUseCamera*, const sead::Vector2f&, float, float);
void calcLineCameraToWorldPosFromScreenPosSub(sead::Vector3f* output, sead::Vector3f*, const al::IUseCamera*, const sead::Vector2f&);

void calcScreenPosFromLayoutPos(sead::Vector2f* output, const sead::Vector2f&);
void calcScreenPosFromWorldPos(sead::Vector2f* output, const al::IUseCamera*, const sead::Vector3f&);
void calcScreenPosFromWorldPosSub(sead::Vector2f* output, const al::IUseCamera*, const sead::Vector3f&);

int calcScreenRadiusFromWorldRadius(const sead::Vector3f&, const al::IUseCamera*, float);     // Return type may be incorrect
int calcScreenRadiusFromWorldRadiusSub(const sead::Vector3f&, const al::IUseCamera*, float);  // Return type may be incorrect

bool calcWorldPosFromScreen(sead::Vector3f* output, const sead::Vector2f&, const sead::Matrix34f&, float);
void calcWorldPosFromScreenPos(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, float);
void calcWorldPosFromScreenPos(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, const sead::Vector3f&);
void calcWorldPosFromScreenPos(sead::Vector3f* output, const al::SceneCameraInfo*, const sead::Vector2f&, float, int);
void calcWorldPosFromScreenPos(sead::Vector3f* output, const al::SceneCameraInfo*, const sead::Vector2f&, const sead::Vector3f&, int);
void calcWorldPosFromScreenPosSub(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, float);
void calcWorldPosFromScreenPosSub(sead::Vector3f* output, const al::IUseCamera*, const sead::Vector2f&, const sead::Vector3f&);

void calcTouchScreenPos(sead::Vector2f* output);

}  // namespace al
