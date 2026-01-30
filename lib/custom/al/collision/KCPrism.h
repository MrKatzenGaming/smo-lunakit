#pragma once

#include <sead/basis/seadTypes.h>
#include <sead/math/seadVector.h>

namespace al {
class KCPrismData {  // triangle
public:
    float mLength;
    u16 mPosIndex;
    u16 mFaceNormalIndex;
    u16 mEdgeNormalIndex[3];
    u16 mCollisionType;
    u32 mTriIndex;
};

class KCPrismHeader {  // model
public:
    u32 mPositionsOffset;
    u32 mNormalsOffset;
    u32 mTrianglesOffset;
    u32 mOctreeOffset;
    float mThickness;
    sead::Vector3f mOctreeOrigin;
    u32 mXMask;
    u32 mYMask;
    u32 mZMask;
    u32 mCoordShift;
    u32 mYShift;
    u32 mZShift;
    float mHitboxRadiusCap;
};
}  // namespace al
