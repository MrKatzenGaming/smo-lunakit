#pragma once

#include "Library/Collision/Collider.h"

namespace al {
struct Triangle;
};

namespace alCollisionUtil {
bool getFirstPolyOnArrow(const al::IUseCollision*, sead::Vector3f* result, al::Triangle* null, const sead::Vector3f& origin,
                         const sead::Vector3f& ray, const al::CollisionPartsFilterBase* nope, const al::TriangleFilterBase* nothanks);

bool getHitPosOnArrow(const al::IUseCollision*, sead::Vector3f*, const sead::Vector3f&, const sead::Vector3f&, const al::CollisionPartsFilterBase*,
                      const al::TriangleFilterBase*);
};  // namespace alCollisionUtil
