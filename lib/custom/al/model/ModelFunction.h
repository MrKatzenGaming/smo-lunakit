#pragma once

#include <math/seadBoundBox.h>

namespace al {
class ModelCtrl;
}

namespace alModelFunction {
void calcBoundingBox(sead::BoundBox3<float>*, const al::ModelCtrl*);
void calcBoundingBoxMtx(sead::Matrix34<float>*, const al::ModelCtrl*);
};  // namespace alModelFunction
