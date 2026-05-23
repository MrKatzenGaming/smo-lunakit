#pragma once

#include "math/seadQuat.h"
#include "math/seadVectorFwd.h"

namespace MathHelper {
sead::Vector3f QuatToEuler(sead::Quatf* quat);

}
