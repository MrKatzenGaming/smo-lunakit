#include "helpers/MathHelper.h"

namespace MathHelper {
sead::Vector3f QuatToEuler(sead::Quatf* quat) {
    // Check for null pointer
    if (!quat) {
        // Handle the error, e.g., return a default value or log an error
        return sead::Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Extract quaternion components
    f32 x = quat->x;
    f32 y = quat->z;
    f32 z = quat->y;
    f32 w = quat->w;

    f32 t0 = 2.0 * (w * x + y * z);
    f32 t1 = 1.0 - 2.0 * (x * x + y * y);
    f32 roll = atan2f(t0, t1);

    f32 t2 = 2.0 * (w * y - z * x);
    t2 = t2 > 1.0 ? 1.0 : t2;
    t2 = t2 < -1.0 ? -1.0 : t2;
    f32 pitch = asinf(t2);

    f32 t3 = 2.0 * (w * z + x * y);
    f32 t4 = 1.0 - 2.0 * (y * y + z * z);
    f32 yaw = atan2f(t3, t4);
    f32 adjustedYaw = yaw;
    if (adjustedYaw < 0)
        adjustedYaw += M_PI * 2;

    return sead::Vector3f(roll, adjustedYaw, pitch);
}
}  // namespace MathHelper
