#pragma once

#include <sead/math/seadVector.h>

#include "Library/Camera/CameraDirector.h"
#include "al/camera/Projection.h"

#include "gfx/seadProjection.h"

namespace al {
sead::Vector3f* getCameraUp(const al::IUseCamera*, int);
sead::Vector3f* getCameraPos(const al::IUseCamera*, int);
void getProjectionMtx(const al::IUseCamera*, int);
void getProjectionMtxPtr(const al::IUseCamera*, int);
sead::Projection* getProjectionSead(const al::IUseCamera*, int);
al::Projection* getProjection(const al::IUseCamera*, int);
}  // namespace al
