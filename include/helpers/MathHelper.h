#pragma once

#include "sead/math/seadQuat.h"
#include "sead/math/seadVectorFwd.h"

namespace MathHelper {
sead::Vector3f QuatToEuler(sead::Quatf* quat);

}
